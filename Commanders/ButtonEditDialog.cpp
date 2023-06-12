#include "ButtonEditDialog.h"
#include "ui_ButtonEditDialog.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRegularExpression>
#include "util/rangeParse.h"
#include "util/DialogUtil.h"
#include <QSet>

ButtonEditDialog::ButtonEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ButtonEditDialog)
{
    ui->setupUi(this);
}

ButtonEditDialog::~ButtonEditDialog()
{
    delete ui;
}

int ButtonEditDialog::exec(){
    ui->textEdit->setText(dialogData.text);
    ui->CommandEdit->setText(QString().number(dialogData.command, 16));
    ui->SubCommandEdit->setText(QString().number(dialogData.subCommand, 16));
    ui->ValueEdit->setText(QString().number(dialogData.value, 16));
    ui->isSaveButtonCheckBox->setChecked(dialogData.isSaveButton);
    ui->areYouSureCheckbox->setChecked(dialogData.asksAreYouSure);
    ui->saveCommandRangeEdit->setText(dialogData.saveRange);
    return QDialog::exec();
}

void ButtonEditDialog::on_isSaveButtonCheckBox_toggled(bool checked)
{
    ui->saveCommandRangeEdit->setEnabled(checked);
    ui->saveCommandRangeLabel->setEnabled(checked);
}

void ButtonEditDialog::done(int r)
{
    if(QDialog::Accepted == r)  // ok was pressed
    {
        dialogData.text = ui->textEdit->text();
        if(!DialogUtil::checkIntLineEdit(ui->CommandEdit, &dialogData.command, 16)) return;
        if(!DialogUtil::checkIntLineEdit(ui->SubCommandEdit, &dialogData.subCommand, 16)) return;
        if(!DialogUtil::checkIntLineEdit(ui->ValueEdit, &dialogData.value, 10)) return;
        dialogData.isSaveButton = ui->isSaveButtonCheckBox->isChecked();
        dialogData.asksAreYouSure = ui->areYouSureCheckbox->isChecked();
        dialogData.saveRange = ui->saveCommandRangeEdit->text();
        if(dialogData.isSaveButton)
        {
            if(dialogData.getSaveCommands().size() == 0)
            {
                auto le = ui->saveCommandRangeEdit;
                le->setFocus();
                le->selectAll();
                return;
            }
        }

        QDialog::done(r);
        return;
    }
    else    // cancel, close or exc was pressed
    {
        QDialog::done(r);
        return;
    }
}

void CommanderButtonData::writeToXml(QXmlStreamWriter &writer) const
{
    writer.writeAttribute("text", text);
    writer.writeAttribute("command", QString().number((uint)command, 16));
    writer.writeAttribute("subCommand", QString().number((uint)subCommand, 16));
    writer.writeAttribute("value", QString().number(value));
    writer.writeAttribute("isSaveButton", QString().number(isSaveButton));
    writer.writeAttribute("asksAreYouSure", QString().number(asksAreYouSure));
    writer.writeAttribute("saveRange", saveRange);
}

void CommanderButtonData::readFromXml(QXmlStreamReader &reader)
{
    text = reader.attributes().value("text").toString();
    command = reader.attributes().value("command").toString().toInt(0,16);
    subCommand = reader.attributes().value("subCommand").toString().toInt(0,16);
    value = reader.attributes().value("value").toString().toInt();
    isSaveButton = reader.attributes().value("isSaveButton").toString().toInt();
    asksAreYouSure = reader.attributes().value("asksAreYouSure").toString().toInt();
    saveRange = reader.attributes().value("saveRange").toString();
    reader.readElementText();
}

QByteArray CommanderButtonData::getSaveCommands()
{
    QByteArray ba;
    auto range = rangeParse(saveRange);
    foreach (int x, range) {
        ba.append((char)x);
    }

    return ba;
}
