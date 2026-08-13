#include "screencopy_qml_view.h"

#include <qobject.h>
#include <qquickitem.h>
#include <qsgnode.h>
#include <qsgtexture.h>

#include "manager.h"
#include "qsg.h"
#include "screencopy_context.h"
#include "screencopy_manager.h"

namespace ns::wayland::screencopy {
ScreencopyQMLView::ScreencopyQMLView(QQuickItem *parent) : QQuickItem(parent) {}

void ScreencopyQMLView::componentComplete() {
  QQuickItem::componentComplete(); //

  auto *bufManager = buffer::WlBufferManager::instance();
  if (!bufManager->isReady()) {
    QObject::connect(bufManager,
                     &buffer::WlBufferManager::ready,
                     this,
                     &ScreencopyQMLView::onBuffersReady);

    bufManager->initWindow(this->window());
  } else {
    this->onBuffersReady();
  }
}

bool ScreencopyQMLView::isLive() const { return m_isLive; }
void ScreencopyQMLView::setIsLive(bool value) {
  if (m_isLive == value) return;

  if (value && !m_isLive && m_context) {
    captureFrameUnsafe();
  }

  m_isLive = value;
  emit isLiveChanged();
}

QObject *ScreencopyQMLView::captureSource() const { return m_captureSource; }
void     ScreencopyQMLView::setCaptureSource(QObject *source) {
  if (source == m_captureSource) return;

  auto previousContext = m_context != nullptr;
  this->deleteContext(false);

  m_captureSource = source;

  if (source) {
    QObject::connect(source,
                     &QObject::destroyed,
                     this,
                     &ScreencopyQMLView::onCaptureSourceDestroyed);

    if (m_isCompleted) {
      this->createContext();
    }
  }

  m_captureSource = source;
  emit captureSourceChanged();
}

void ScreencopyQMLView::createContext() {
  this->deleteContext(false);

  m_context = ScreencopyManager::createContext(this->m_captureSource);

  if (!m_context) {
    return;
  }

  m_context->setParent(this);

  QObject::connect(m_context,
                   &ScreencopyContext::stopped,
                   this,
                   &ScreencopyQMLView::onContextStopped);

  QObject::connect(m_context,
                   &ScreencopyContext::frameCaptured,
                   this,
                   &ScreencopyQMLView::onFrameCaptured);
}

void ScreencopyQMLView::captureFrame() {
  if (m_context) {
    captureFrameUnsafe();
  }
}

void ScreencopyQMLView::onContextStopped() { this->deleteContext(true); }

void ScreencopyQMLView::onFrameCaptured() {
  this->setFlag(QQuickItem::ItemHasContents);
  this->update();

  const auto &frontBuffer = m_context->swapchain().frontBuffer();

  auto size = frontBuffer->size();
  if (frontBuffer->transform.flipSize()) size.transpose();

  b_hasContent = true;
}

void ScreencopyQMLView::onCaptureSourceDestroyed() {
  m_captureSource = nullptr;
  this->deleteContext();
}

void ScreencopyQMLView::onBuffersReady() {
  m_isCompleted = true;
  if (m_captureSource) this->createContext();
}

void ScreencopyQMLView::deleteContext(bool shouldUpdate) {
  auto previousContext = m_context != nullptr;
  if (previousContext) {
    delete m_context;
  }

  m_context    = nullptr;
  b_hasContent = false;

  if (previousContext && shouldUpdate) {
    this->update();
  }
}

QSGNode *ScreencopyQMLView::updatePaintNode(QSGNode *oldNode,
                                            UpdatePaintNodeData * /*unused*/) {
  if (!m_context || !b_hasContent) {
    delete oldNode;
    this->setFlag(QQuickItem::ItemHasContents, false);
    return nullptr;
  }

  auto *node = static_cast<buffer::WlBufferQSGDisplayNode *>(oldNode);

  if (!node) {
    node = new buffer::WlBufferQSGDisplayNode(this->window());
  }

  auto &swapchain = m_context->swapchain();
  node->syncSwapchain(swapchain);
  node->setRect(this->boundingRect());
  node->setFiltering(QSGTexture::Linear);

  if (m_isLive) {
    captureFrameUnsafe();
  }

  return node;
}
} // namespace ns::wayland::screencopy
