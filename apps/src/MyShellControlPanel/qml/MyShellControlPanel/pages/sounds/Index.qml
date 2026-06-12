pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellPlugin.Utils
import MyShellControlPanel.components
import MyShellControlPanel.components.forms
import MyShellControlPanel.plugin
import QtQuick
import QtQuick.Layouts

PageStackItem {
  id: root
  title: "Sounds Config"

  FormController {
	id: soundThemeController
	models: [Config.sounds] // qmllint disable missing-type
  }

  SoundThemes {
	id: sthemes

	property list<var> vals: themes.map(theme => {
	  return {
		value: theme.xdgName,
		text: theme.themeName
	  };
	})
  }

  ColumnLayout {
	id: mainLayout

	anchors.top: parent.top
	anchors.left: parent.left
	anchors.right: parent.right

	spacing: Config.appearance.spacing.md

	StyledText {
	  id: description

	  Layout.fillWidth: true

	  font.family: Config.appearance.fontFamily.sans
	  font.pointSize: Config.appearance.fontSize.sm

	  wrapMode: Text.WordWrap
	  textFormat: Text.MarkdownText

    // qmlformat off
    text: `# System sounds` +
    `\n\nChanges system sounds, according to XDG Sound Theme specifications.`
    // qmlformat on
	}

	SCheckBox {
	  id: soundThemesEnabled
	  name: "Enabled"

	  AutoFormField {
		controller: soundThemeController
		field: ["SoundsConfig", "enabled"]
	  }
	}

	SSelector {
	  id: soundThemeSelector
	  model: sthemes.vals
	  name: "Theme name"

	  control.textRole: "text"
	  control.valueRole: "value"

	  function modelFindIndex(val: string): int {
		return model.findIndex(v => v.value == val);
	  }

	  function modelGetName(item: var): string {
		return item.value;
	  }

	  AutoFormField {
		controller: soundThemeController
		field: ["SoundsConfig", "theme"]
	  }
	}

	FormButtons {
	  id: soundThemeButtons
	  controller: soundThemeController
	}
  }
}
