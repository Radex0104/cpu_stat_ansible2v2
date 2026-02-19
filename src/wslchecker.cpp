#include "wslchecker.h"
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>

// Constructor
WSLChecker::WSLChecker(QObject *parent) : QObject(parent)
{
    m_installProcess = new QProcess(this);
    
    connect(m_installProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onInstallProcessFinished(int, QProcess::ExitStatus)));
    
    qDebug() << "WSLChecker created";
}

// Destructor
WSLChecker::~WSLChecker()
{
    if (m_installProcess && m_installProcess->state() != QProcess::NotRunning) {
        m_installProcess->terminate();
        m_installProcess->waitForFinished(1000);
    }
    qDebug() << "WSLChecker destroyed";
}

// SYNCHRONOUS WSL check - SIMPLIFIED VERSION
WSLChecker::WSLInfo WSLChecker::checkWSL()
{
    qDebug() << "Starting simplified WSL check...";
    
    WSLInfo info;
    
    // SIMPLE CHECK: Try to run echo command inside WSL
    QProcess testProcess;
    testProcess.start("wsl", QStringList() << "echo" << "WSL_TEST");
    
    if (testProcess.waitForFinished(3000)) {
        QString output = testProcess.readAllStandardOutput().trimmed();
        qDebug() << "WSL echo test output:" << output;
        
        if (output == "WSL_TEST") {
            // WSL is working and has a distribution
            qDebug() << "WSL is working and has a distribution";
            info.isInstalled = true;
            info.hasDistributions = true;
            info.distributions.append("Ubuntu (detected)");
            info.defaultDistribution = "Ubuntu";
        } else {
            // WSL is installed but no distribution is running
            qDebug() << "WSL is installed but no distribution is running";
            info.isInstalled = true;
            info.hasDistributions = false;
            info.errorMessage = "WSL installed but no distribution";
        }
    } else {
        // Check if wsl.exe exists
        QProcess whichProcess;
        whichProcess.start("where", QStringList() << "wsl.exe");
        
        if (whichProcess.waitForFinished(2000)) {
            QString whichOutput = whichProcess.readAllStandardOutput();
            if (!whichOutput.isEmpty()) {
                qDebug() << "wsl.exe found but not responding";
                info.isInstalled = true;
                info.hasDistributions = false;
                info.errorMessage = "WSL installed but not responding";
            } else {
                qDebug() << "wsl.exe not found";
                info.isInstalled = false;
                info.errorMessage = "WSL not installed";
            }
        } else {
            info.isInstalled = false;
            info.errorMessage = "WSL not installed";
        }
    }
    
    m_lastInfo = info;
    
    qDebug() << "WSL check completed:";
    qDebug() << "  isInstalled:" << info.isInstalled;
    qDebug() << "  hasDistributions:" << info.hasDistributions;
    qDebug() << "  errorMessage:" << info.errorMessage;
    
    return info;
}

// Show WSL setup dialog
void WSLChecker::showWslSetupDialog()
{
    qDebug() << "Showing WSL setup dialog";
    qDebug() << "  isInstalled:" << m_lastInfo.isInstalled;
    qDebug() << "  hasDistributions:" << m_lastInfo.hasDistributions;
    
    if (!m_lastInfo.isInstalled) {
        showWslInstallDialog();
    } else if (!m_lastInfo.hasDistributions) {
        showWslDistroDialog();
    } else {
        qDebug() << "WSL is properly configured";
    }
}

// SLOT: installation finished
void WSLChecker::onInstallProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    
    qDebug() << "Installation process finished, exitCode:" << exitCode;
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    if (exitCode == 0) {
        qDebug() << "Installation successful";
        QMessageBox::information(parent, "Installation Complete", 
            "Installation completed successfully! Please restart your computer.");
        emit wslSetupFinished(true);
    } else {
        QString error = m_installProcess->readAllStandardError();
        qDebug() << "Installation failed, error:" << error;
        QMessageBox::warning(parent, "Installation Error", 
            "Failed to install WSL.\n\n" + error);
        emit wslSetupFinished(false);
    }
}

// Dialog for WSL installation
void WSLChecker::showWslInstallDialog()
{
    qDebug() << "Showing WSL installation dialog";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("WSL Not Found");
    msgBox.setText("This program requires Windows Subsystem for Linux (WSL).");
    msgBox.setInformativeText("WSL was not found on this computer. Would you like to install WSL?");
    
    QPushButton *installButton = msgBox.addButton("Install WSL", QMessageBox::AcceptRole);
    QPushButton *manualButton = msgBox.addButton("Installation Guide", QMessageBox::HelpRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == installButton) {
        qDebug() << "User chose to install WSL";
        installWsl();
    } else if (msgBox.clickedButton() == manualButton) {
        qDebug() << "User chose to view installation guide";
        openWslInstallationGuide();
    } else {
        qDebug() << "User cancelled WSL installation";
    }
}

// Dialog for distribution selection
void WSLChecker::showWslDistroDialog()
{
    qDebug() << "Showing distribution selection dialog";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QDialog dialog(parent);
    dialog.setWindowTitle("Choose WSL Distribution");
    dialog.setMinimumWidth(450);
    dialog.setMinimumHeight(350);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QLabel *infoLabel = new QLabel(
        "<b>WSL is installed but no Linux distributions were found.</b><br><br>"
        "This program requires a Linux distribution. "
        "Select a distribution from the list to install.", 
        &dialog);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    
    QLabel *listLabel = new QLabel("Popular distributions:", &dialog);
    layout->addWidget(listLabel);
    
    QListWidget *distroList = new QListWidget(&dialog);
    
    QStringList popularDistros = {
        "Ubuntu",
        "Ubuntu-22.04",
        "Ubuntu-20.04",
        "Debian",
        "kali-linux",
        "Alpine",
        "Fedora",
        "openSUSE-42"
    };
    
    for (const QString &distro : popularDistros) {
        QListWidgetItem *item = new QListWidgetItem(distro);
        distroList->addItem(item);
    }
    
    distroList->setCurrentRow(0);
    distroList->setMinimumHeight(150);
    layout->addWidget(distroList);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(&dialog);
    QPushButton *installButton = buttonBox->addButton("Install", QDialogButtonBox::AcceptRole);
    QPushButton *cancelButton = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
    
    layout->addWidget(buttonBox);
    
    QObject::connect(installButton, &QPushButton::clicked, [&]() {
        if (distroList->currentItem()) {
            QString distro = distroList->currentItem()->text();
            qDebug() << "User selected distribution:" << distro;
            dialog.accept();
            installWslDistro(distro);
        } else {
            QMessageBox::warning(&dialog, "Error", "Please select a distribution to install");
        }
    });
    
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    dialog.exec();
}

// Install WSL
void WSLChecker::installWsl()
{
    qDebug() << "Starting WSL installation";
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox::information(parent, "Installing WSL", 
        "Starting WSL installation. This may take several minutes.\n"
        "A system restart may be required after installation.");
    
    QStringList args;
    args << "/c" << "wsl" << "--install";
    
    m_installProcess->start("cmd.exe", args);
    qDebug() << "Installation process started with args:" << args;
}

// Install specific WSL distribution
void WSLChecker::installWslDistro(const QString &distroName)
{
    qDebug() << "Starting distribution installation:" << distroName;
    
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    
    QMessageBox::information(parent, "Installing Distribution", 
        QString("Starting installation of %1. This may take several minutes.").arg(distroName));
    
    QStringList args;
    args << "/c" << "wsl" << "--install" << "-d" << distroName;
    
    m_installProcess->start("cmd.exe", args);
    qDebug() << "Installation process started with args:" << args;
}

// Open WSL installation guide
void WSLChecker::openWslInstallationGuide()
{
    qDebug() << "Opening WSL installation guide in browser";
    QDesktopServices::openUrl(QUrl("https://docs.microsoft.com/en-us/windows/wsl/install"));
}