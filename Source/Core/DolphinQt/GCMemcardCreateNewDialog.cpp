// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DolphinQt/GCMemcardCreateNewDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>

#include "Common/FileUtil.h"
#include "Common/MsgHandler.h"

#include "Core/HW/GCMemcard/GCMemcard.h"

GCMemcardCreateNewDialog::GCMemcardCreateNewDialog(QWidget* parent) : QDialog(parent)
{
  m_combobox_size = new QComboBox();
  m_combobox_size->addItem(tr("4 Mbit (59 blocks)"), 4);
  m_combobox_size->addItem(tr("8 Mbit (123 blocks)"), 8);
  m_combobox_size->addItem(tr("16 Mbit (251 blocks)"), 16);
  m_combobox_size->addItem(tr("32 Mbit (507 blocks)"), 32);
  m_combobox_size->addItem(tr("64 Mbit (1019 blocks)"), 64);
  m_combobox_size->addItem(tr("128 Mbit (2043 blocks)"), 128);
  m_combobox_size->setCurrentIndex(5);

  m_radio_western = new QRadioButton(tr("Western (Windows-1252)"));
  m_radio_shiftjis = new QRadioButton(tr("Japanese (Shift-JIS)"));
  m_radio_western->setChecked(true);

  auto* card_size_label = new QLabel(tr("Card Size"));
  auto* card_encoding_label = new QLabel(tr("Encoding"));
  auto* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  button_box->button(QDialogButtonBox::Ok)->setText(tr("Create..."));

  auto* layout = new QGridLayout();
  layout->addWidget(card_size_label, 0, 0);
  layout->addWidget(m_combobox_size, 0, 1);
  layout->addWidget(card_encoding_label, 1, 0);
  layout->addWidget(m_radio_western, 1, 1);
  layout->addWidget(m_radio_shiftjis, 2, 1);
  layout->addWidget(button_box, 3, 0, 1, 2, Qt::AlignRight);
  setLayout(layout);

  connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(button_box, &QDialogButtonBox::accepted, [this] {
    if (CreateCard())
      accept();
  });

  setWindowTitle(tr("Create New Memory Card"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

GCMemcardCreateNewDialog::~GCMemcardCreateNewDialog() = default;

bool GCMemcardCreateNewDialog::CreateCard()
{
  const u16 size = static_cast<u16>(m_combobox_size->currentData().toInt());
  const bool is_shift_jis = m_radio_shiftjis->isChecked();

  const QString path = QFileDialog::getSaveFileName(
      this, tr("Create New Memory Card"), QString::fromStdString(File::GetUserPath(D_GCUSER_IDX)),
      tr("GameCube Memory Cards (*.raw *.gcp)") + QStringLiteral(";;") + tr("All Files (*)"));

  if (path.isEmpty())
    return false;

  const std::string p = path.toStdString();
  auto memcard = GCMemcard::Create(p, size, is_shift_jis);
  if (memcard && memcard->Save())
  {
    m_card_path = p;
    return true;
  }

  return false;
}

std::string GCMemcardCreateNewDialog::GetMemoryCardPath() const
{
  return m_card_path;
}