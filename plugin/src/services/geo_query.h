#pragma once

#include <memory>
#include <vector>

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "ns_services_export.h"

namespace ns::services {
class NS_SERVICES_EXPORT GeolocationResult {
  Q_GADGET
  QML_VALUE_TYPE(geolocationResult)

public:
  GeolocationResult();
  ~GeolocationResult();

  GADGET_PROP(QString, name)
  GADGET_PROP(QString, country)
  GADGET_PROP(QString, country_code)
  GADGET_PROP(QString, timezone)
  GADGET_PROP(QString, admin1)
  GADGET_PROP(qreal, latitude)
  GADGET_PROP(qreal, longitude)
};

class NS_SERVICES_EXPORT GeolocationQuery : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
  Q_PROPERTY(bool isPending READ default NOTIFY isPendingChanged BINDABLE
          bindableIsPending)
  // Q_PROPERTY(QQmlListProperty<ns::services::GeolocationResult> results READ
  //         results NOTIFY resultsChanged)
  Q_PROPERTY(
      QList<GeolocationResult> *results READ results NOTIFY resultsChanged)

  using RESULT_QML_LIST = QQmlListProperty<GeolocationResult>;
  using RESULT_LIST     = QList<GeolocationResult>;

public:
  explicit GeolocationQuery(QObject *parent = nullptr);
  ~GeolocationQuery() override;

  [[nodiscard]] QBindable<bool> bindableIsPending() const {
    return &b_isPending;
  }

  static qsizetype          geo_list_count(RESULT_QML_LIST *property);
  static GeolocationResult *geo_list_at(
      RESULT_QML_LIST *property, qsizetype index);

  // [[nodiscard]] RESULT_QML_LIST results();
  [[nodiscard]] RESULT_LIST *results() const;

  [[nodiscard]] QString query() const;
  void                  setQuery(const QString &value);

signals:
  void queryChanged();
  void isPendingChanged();
  void resultsChanged();

private slots:
  void onDebounceTimeout();

private:
  bool                         m_queuedQuery = false;
  std::unique_ptr<RESULT_LIST> m_results;
  QString                      m_query;
  QTimer                       m_debouceTimer;

  Q_OBJECT_BINDABLE_PROPERTY(
      GeolocationQuery, bool, b_isPending, &GeolocationQuery::isPendingChanged)
};
} // namespace ns::services
