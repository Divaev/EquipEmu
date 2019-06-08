object MakeFileForm: TMakeFileForm
  Left = 500
  Top = 537
  Width = 423
  Height = 376
  Caption = 'Создание файла режима'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object BKSFileLabel: TLabel
    Left = 16
    Top = 16
    Width = 112
    Height = 13
    Caption = 'Файл протокола БКС:'
  end
  object BKIFileLabel: TLabel
    Left = 16
    Top = 72
    Width = 110
    Height = 13
    Caption = 'Файл протокола БКИ'
  end
  object SystemNameLabel: TLabel
    Left = 16
    Top = 128
    Width = 127
    Height = 13
    Caption = 'Наименование системы:'
  end
  object ModNameLabel: TLabel
    Left = 16
    Top = 184
    Width = 122
    Height = 13
    Caption = 'Наименование режима:'
  end
  object BKSFileImg: TImage
    Left = 384
    Top = 32
    Width = 25
    Height = 21
    Stretch = True
    Transparent = True
  end
  object BKIFileImg: TImage
    Left = 384
    Top = 88
    Width = 25
    Height = 21
    Stretch = True
    Transparent = True
  end
  object SystemNameImg: TImage
    Left = 312
    Top = 144
    Width = 25
    Height = 21
    Stretch = True
    Transparent = True
  end
  object ModNameImg: TImage
    Left = 312
    Top = 200
    Width = 25
    Height = 21
    Stretch = True
    Transparent = True
  end
  object ResultFileImg: TImage
    Left = 384
    Top = 256
    Width = 25
    Height = 21
    Stretch = True
    Transparent = True
  end
  object SaveFileLabel: TLabel
    Left = 16
    Top = 240
    Width = 89
    Height = 13
    Caption = 'Созданный файл:'
  end
  object BKSFilePath: TEdit
    Left = 16
    Top = 32
    Width = 281
    Height = 21
    TabOrder = 0
    Text = 'Укажите путь к файлу'
  end
  object BKIFilePath: TEdit
    Left = 16
    Top = 88
    Width = 281
    Height = 21
    TabOrder = 1
    Text = 'Укажите путь к файлу'
  end
  object SystemNameEdit: TEdit
    Left = 16
    Top = 144
    Width = 281
    Height = 21
    TabOrder = 2
    Text = 'Введите наименование системы'
    OnChange = SystemNameChange
  end
  object ModNameEdit: TEdit
    Left = 16
    Top = 200
    Width = 281
    Height = 21
    TabOrder = 3
    Text = 'Введите наименование режима'
    OnChange = ModNameChange
  end
  object BKSFileButt: TButton
    Left = 304
    Top = 32
    Width = 75
    Height = 21
    Caption = 'Обзор...'
    TabOrder = 4
    OnClick = BKSFileButtClick
  end
  object BKIFileButt: TButton
    Left = 304
    Top = 88
    Width = 75
    Height = 21
    Caption = 'Обзор...'
    TabOrder = 5
    OnClick = BKIFileButtClick
  end
  object ApplySetsButt: TButton
    Left = 152
    Top = 304
    Width = 121
    Height = 25
    Caption = 'Принять'
    TabOrder = 6
    OnClick = ApplySetsButtClick
  end
  object ResultFilePath: TEdit
    Left = 16
    Top = 256
    Width = 281
    Height = 21
    TabOrder = 7
    Text = 'Выберите директорию для сохранения'
  end
  object ResultFileButt: TButton
    Left = 304
    Top = 256
    Width = 75
    Height = 21
    Caption = 'Обзор...'
    TabOrder = 8
    OnClick = ResultFileButtClick
  end
  object OpenFileBKS: TOpenDialog
    Left = 384
    Top = 8
  end
  object OpenFileBKI: TOpenDialog
    Left = 384
    Top = 56
  end
  object SaveResultFile: TSaveDialog
    Left = 384
    Top = 224
  end
end
