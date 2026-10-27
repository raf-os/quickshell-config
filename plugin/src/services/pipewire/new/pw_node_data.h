#pragma once

#include <pipewire/node.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/pod/pod.h>

#include "pw_common.h"
namespace ns::pipewire {
class Node;

class INodeData {
public:
  virtual ~INodeData() {}

  virtual void onDeviceChanged()                     = 0;
  virtual void onInfo(const pw_node_info * /*info*/) = 0;
  virtual void onSpaParam(
      quint32 /*id*/, quint32 /*index*/, const spa_pod * /*param*/) = 0;
};

class NodeAudioData : public QObject, public INodeData {
  Q_OBJECT

public:
  explicit NodeAudioData(Node *parent);
  ~NodeAudioData() override;

  void onDeviceChanged() override;
  void onInfo(const pw_node_info *info) override;
  void onSpaParam(quint32 id, quint32 index, const spa_pod *param) override;

  [[nodiscard]] bool isMuted() const;
  void               setMuted(bool muted);

  [[nodiscard]] QList<AudioChannel::Enum> channels() const;

  [[nodiscard]] QList<float> volumes() const;
  void                       setVolumes(const QList<float> &values);

  [[nodiscard]] float averageVolume() const;
  void                setAverageVolume(float value);

signals:
  void mutedChanged();
  void volumesChanged();
  void channelsChanged();

private slots:
  void onDeviceReady();
  void onDeviceVolumesChanged(
      qint32 routeDevice, const VolumeProps *volumeProps);

private:
  Node *m_parent;

  bool  m_muted      = false;
  float m_volumeStep = -1;

  QList<AudioChannel::Enum> m_channels;
  QList<float>              m_volumes;
  QList<float>              m_serverVolumes;
  QList<float>              m_deviceVolumes;
  QList<float>              m_queuedVolumes;

  void updateVolumeProps(const VolumeProps *volumeProps);
};
} // namespace ns::pipewire
