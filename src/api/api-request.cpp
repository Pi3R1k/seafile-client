#include <QtNetwork>

#include "api-request.h"
#include "api-client.h"

SeafileApiRequest::SeafileApiRequest(const QUrl& url, Method method,
                                     const QString& token, bool ignore_ssl_errors)
    : url_(url),
      method_(method),
      token_(token),
      ignore_ssl_errors_(ignore_ssl_errors)
{
    api_client_ = new SeafileApiClient;
}

SeafileApiRequest::~SeafileApiRequest()
{
    delete api_client_;
}

void SeafileApiRequest::setParam(const QString& name, const QString& value)
{
    params_.addEncodedQueryItem(QUrl::toPercentEncoding(name),
                                QUrl::toPercentEncoding(value));
}

void SeafileApiRequest::send()
{
    if (token_.size() > 0) {
        api_client_->setToken(token_);
    }

    switch (method_) {
    case METHOD_GET:
        api_client_->get(url_);
        break;
    case METHOD_POST:
        api_client_->post(url_, params_.encodedQuery());
        break;
    }

    connect(api_client_, SIGNAL(requestSuccess(QNetworkReply&)),
            this, SLOT(requestSuccess(QNetworkReply&)));

    connect(api_client_, SIGNAL(requestFailed(int)),
            this, SIGNAL(failed(int)));

    connect(api_client_, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)),
            this, SLOT(onSslErrors(QNetworkReply*, const QList<QSslError>&)));
            
}

void SeafileApiRequest::onSslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
    if (ignore_ssl_errors_) {
        reply->ignoreSslErrors();
    } else {
        emit sslErrors(reply, errors);
    }
}

json_t* SeafileApiRequest::parseJSON(QNetworkReply &reply, json_error_t *error)
{
    QByteArray raw = reply.readAll();
    //qDebug("\n%s\n", raw.data());
    json_t *root = json_loads(raw.data(), 0, error);
    return root;
}
