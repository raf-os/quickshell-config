import org.nightshell.Wayland
import org.nightshell.Services
import org.nightshell.Services.Pam

import QtQuick

Item {
	id: root

	required property SessionLockManager lockManager
	property string buffer
	property string message
	property bool messageIsError
	property bool pending: false

	property list<string> messageBuffer: []

	property bool locked
	property bool shouldLockout: false
	property bool lockout: false
	property int lockoutTimeLeft: 10

	signal unlockRequested

	Timer {
		id: failTimeout
		interval: 60 * 1000 // update every 1 minute
		running: root.lockout
		onTriggered: {
			root.lockoutTimeLeft -= 1;
			if (root.lockoutTimeLeft <= 0) {
				root.lockout = false;
			}
		}
	}

	Timer {
		id: messageDismissTimeout
		interval: 4000
		onTriggered: {
			root.message = "";
		}
	}

	onMessageChanged: {
		messageDismissTimeout.restart();
	}

	function start() {
		shouldLockout = false;
		message = "";
		pamContext.start();
	}

	function abort() {
		pamContext.abort();
	}

	PamContext {
		id: pamContext

		onPamMessageReceived: {
			if (message === "" || !isResponseVisible || message.startsWith("Password:"))
				return;
			root.messageBuffer.push(message);
			const match = message.match(/\((\d+) minutes left to unlock\)/);
			if (match) {
				root.shouldLockout = true;
				root.lockoutTimeLeft = parseInt(match[1], 10);
			} else if (message.includes("account is locked due to")) {
				root.shouldLockout = true;
			}
		}

		function getAllMessages(): string {
			const finalMessage = root.messageBuffer.join("\n");
			root.messageBuffer = [];
			return finalMessage;
		}

		onIsResponseRequiredChanged: {
			if (!isResponseRequired)
				return;

			root.pending = true;
			respond(root.buffer);
			root.buffer = "";
		}

		onCompleted: result => {
			root.pending = false;
			const finalMessage = getAllMessages();

			if (root.shouldLockout)
				root.lockout = true;

			switch (result) {
			case PamResult.Success:
				root.messageIsError = false;
				return root.unlockRequested();
			case PamResult.Failed:
				if (finalMessage !== "")
					root.message = finalMessage;
				else
					root.message = "Authentication failed.";
				root.messageIsError = true;
				return;
			case PamResult.Error:
				root.message = "Authentication error.";
				root.messageIsError = true;
				return;
			case PamResult.MaxTries:
				root.message = "Maximum amount of tries reached. Session is locked out.";
				root.messageIsError = true;
				return;
			}
		}

		onError: result => {
			root.pending = false;
			root.messageIsError = true;
			getAllMessages();
			if (root.shouldLockout)
				root.lockout = true;

			switch (result) {
			case PamError.StartFailed:
				root.message = "Failed starting PAM module.";
				return;
			case PamError.TryAuthFailed:
				root.message = "PAM authentication module failed.";
				return;
			case PamError.InternalError:
				root.message = "Internal error occurred.";
				return;
			}
		}
	}

	Connections {
		target: root.lockManager
		function onIsSecureChanged() {
			if (root.lockManager.isSecure) {
				root.buffer = "";
				root.message = "";
				root.messageIsError = false;
			}
		}
	}
}
