#pragma once
#include <QDialog>

class QLineEdit;

class ConnectDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

    QString serverUrl()   const;
    QString displayName() const;

private slots:
    void onConnect();

private:
    QLineEdit *m_urlInput;
    QLineEdit *m_nameInput;
};
