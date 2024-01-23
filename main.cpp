#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QProgressDialog>
#include <QTimer>
#include <QFile>
#include <QTextStream>

class PatientQueueApp : public QWidget {
    Q_OBJECT

public:
    PatientQueueApp(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        // Экран регистрации
        registerScreen = new QWidget(this);
        registerLayout = new QVBoxLayout(registerScreen);
        registerButton = new QPushButton("Зарегистрировать пациента", this);
        connect(registerButton, &QPushButton::clicked, this, &PatientQueueApp::registerPatient);
        registerButton->setFont(QFont("Arial", 12, QFont::Bold));
        registerLayout->addWidget(registerButton);
        layout->addWidget(registerScreen);

        // Экран приема
        receptionScreen = new QWidget(this);
        receptionLayout = new QVBoxLayout(receptionScreen);
        receptionList = new QListWidget(this);
        receptionButton = new QPushButton("Прием", this);
        connect(receptionButton, &QPushButton::clicked, this, &PatientQueueApp::processPatient);
        receptionButton->setFont(QFont("Arial", 12, QFont::Bold));
        receptionLayout->addWidget(receptionList);
        receptionLayout->addWidget(receptionButton);
        layout->addWidget(receptionScreen);

        // Экран очереди
        queueScreen = new QWidget(this);
        queueLayout = new QVBoxLayout(queueScreen);
        queueList = new QListWidget(this);
        backButton = new QPushButton("Назад", this);
        connect(backButton, &QPushButton::clicked, this, &PatientQueueApp::goBackToRegistration);
        processButton = new QPushButton("Прием", this);
        connect(processButton, &QPushButton::clicked, this, &PatientQueueApp::processSelectedPatient);
        registerInQueueButton = new QPushButton("Регистрация", this);
        connect(registerInQueueButton, &QPushButton::clicked, this, &PatientQueueApp::registerPatientInQueue);
        backButton->setFont(QFont("Arial", 12, QFont::Bold));
        processButton->setFont(QFont("Arial", 12, QFont::Bold));
        registerInQueueButton->setFont(QFont("Arial", 12, QFont::Bold));
        queueLayout->addWidget(queueList);
        queueLayout->addWidget(backButton);
        queueLayout->addWidget(processButton);
        queueLayout->addWidget(registerInQueueButton);
        layout->addWidget(queueScreen);

        // Окно ожидания
        progressDialog = new QProgressDialog("Ожидание...", "Отмена", 0, 0, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setCancelButton(nullptr);
        progressDialog->hide();

        // Инициализация
        currentScreen = registerScreen;
        switchScreen(currentScreen);

        // Загрузка данных из файла при запуске программы
        loadPatientData();

        setLayout(layout);
    }

    ~PatientQueueApp() {
        // Сохранение данных в файл при закрытии программы
        savePatientData();
    }

private slots:
    void registerPatient() {
        bool ok;
        QString patientName = QInputDialog::getText(this, "Регистрация", "Введите имя пациента:", QLineEdit::Normal, "", &ok);

        if (ok && !patientName.isEmpty()) {
            // Показываем окно ожидания
            progressDialog->show();

            // Запускаем таймер для имитации процесса регистрации
            QTimer::singleShot(500, this, [this, patientName]() {
                // Закрываем окно ожидания
                progressDialog->hide();

                // Добавляем пациента в очередь с правильным номером
                int queueNumber = calculateQueueNumber();
                queueList->addItem(QString("%1. %2").arg(queueNumber).arg(patientName));

                // Переключаемся на экран очереди
                switchScreen(queueScreen);
            });
        }
    }

    void processPatient() {
        // Переключаемся на экран приема
        switchScreen(receptionScreen);
    }

    void goBackToRegistration() {
        // Переключаемся на экран регистрации
        switchScreen(registerScreen);
    }

    void processSelectedPatient() {
        QList<QListWidgetItem *> selectedItems = queueList->selectedItems();
        for (QListWidgetItem *item : selectedItems) {
            delete item;
        }
        // Пересчитываем номера в очереди после удаления
        recalculateQueueNumbers();
    }

    void registerPatientInQueue() {
        bool ok;
        QString patientName = QInputDialog::getText(this, "Регистрация", "Введите имя пациента:", QLineEdit::Normal, "", &ok);

        if (ok && !patientName.isEmpty()) {
            // Показываем окно ожидания
            progressDialog->show();

            // Запускаем таймер для имитации процесса регистрации
            QTimer::singleShot(500, this, [this, patientName]() {
                // Закрываем окно ожидания
                progressDialog->hide();

                // Добавляем пациента в очередь с правильным номером
                int queueNumber = calculateQueueNumber();
                queueList->addItem(QString("%1. %2").arg(queueNumber).arg(patientName));
            });
        }
    }

private:
    void switchScreen(QWidget *screen) {
        registerScreen->hide();
        receptionScreen->hide();
        queueScreen->hide();

        screen->show();
        currentScreen = screen;
    }

    int calculateQueueNumber() {
        return queueList->count() + 1;
    }

    void recalculateQueueNumbers() {
        for (int i = 0; i < queueList->count(); ++i) {
            QString originalText = queueList->item(i)->text();
            int dotIndex = originalText.indexOf('.');
            QString newText = QString("%1. %2").arg(i + 1).arg(originalText.mid(dotIndex + 2));
            queueList->item(i)->setText(newText);
        }
    }

    void loadPatientData() {
        QFile file("patient_data.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                queueList->addItem(line);
            }
            file.close();
        }
    }

    void savePatientData() {
        QFile file("patient_data.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (int i = 0; i < queueList->count(); ++i) {
                out << queueList->item(i)->text() << "\n";
            }
            file.close();
        }
    }

    QWidget *currentScreen;
    QWidget *registerScreen;
    QWidget *receptionScreen;
    QWidget *queueScreen;

    QVBoxLayout *registerLayout;
    QVBoxLayout *receptionLayout;
    QVBoxLayout *queueLayout;

    QListWidget *receptionList;
    QListWidget *queueList;

    QPushButton *registerButton;
    QPushButton *receptionButton;
    QPushButton *backButton;
    QPushButton *processButton;
    QPushButton *registerInQueueButton;

    QProgressDialog *progressDialog;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PatientQueueApp patientQueueApp;
    patientQueueApp.resize(800, 600);
    patientQueueApp.show();

    return app.exec();
}

#include "main.moc"
