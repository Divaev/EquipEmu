object Form1: TForm1
  Left = 253
  Top = 249
  Width = 745
  Height = 532
  Caption = 'System Emulator'
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
  object BarBKSLabel: TLabel
    Left = 8
    Top = 400
    Width = 41
    Height = 20
    Caption = 'БКС:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object BarBKILabel: TLabel
    Left = 8
    Top = 432
    Width = 41
    Height = 20
    Caption = 'БКИ:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PitButt: TButton
    Left = 64
    Top = 464
    Width = 153
    Height = 33
    Caption = 'Подать питание'
    TabOrder = 0
    OnClick = PitButtClick
  end
  object Info: TGroupBox
    Left = 376
    Top = 8
    Width = 353
    Height = 233
    Caption = 'Информация'
    TabOrder = 1
    object SystemNameLabel: TLabel
      Left = 16
      Top = 24
      Width = 176
      Height = 20
      Caption = 'Система не выбрана'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -17
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ModNameLabel: TLabel
      Left = 16
      Top = 56
      Width = 148
      Height = 20
      Caption = 'Режим не выбран'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -17
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object StatusLabel: TLabel
      Left = 16
      Top = 88
      Width = 240
      Height = 16
      Caption = 'Статус: питание не подключено'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object TimerGroup: TGroupBox
      Left = 8
      Top = 120
      Width = 337
      Height = 50
      Caption = 'Таймер'
      TabOrder = 0
      object TimerBKSLabel: TLabel
        Left = 40
        Top = 17
        Width = 46
        Height = 16
        Caption = 'БКС: 0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object TimerBKILabel: TLabel
        Left = 198
        Top = 17
        Width = 51
        Height = 16
        Caption = ' БКИ: 0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object SignalsGroup: TGroupBox
      Left = 8
      Top = 176
      Width = 337
      Height = 50
      Caption = 'Сигналы'
      TabOrder = 1
      object SentBKSLabel: TLabel
        Left = 40
        Top = 17
        Width = 59
        Height = 16
        Caption = 'БКС: 0/0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object SentBKILabel: TLabel
        Left = 203
        Top = 17
        Width = 60
        Height = 16
        Caption = 'БКИ: 0/0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
  end
  object SigBarBKS: TProgressBar
    Left = 56
    Top = 432
    Width = 673
    Height = 17
    Min = 0
    Max = 100
    Step = 1
    TabOrder = 2
  end
  object SigBarBKI: TProgressBar
    Left = 56
    Top = 400
    Width = 673
    Height = 17
    Min = 0
    Max = 100
    Step = 1
    TabOrder = 3
  end
  object PauseButt: TButton
    Left = 336
    Top = 464
    Width = 153
    Height = 33
    Caption = 'Приостановить'
    TabOrder = 4
    OnClick = PauseButtClick
  end
  object NextButt: TButton
    Left = 576
    Top = 464
    Width = 153
    Height = 33
    Caption = 'Следующий сигнал'
    TabOrder = 5
    OnClick = NextButtClick
  end
  object RichBKS: TRichEdit
    Left = 8
    Top = 256
    Width = 353
    Height = 129
    HideScrollBars = False
    Lines.Strings = (
      'RichBKS')
    ScrollBars = ssVertical
    TabOrder = 6
  end
  object RichBKI: TRichEdit
    Left = 376
    Top = 256
    Width = 353
    Height = 129
    HideScrollBars = False
    Lines.Strings = (
      'RichBKI')
    ScrollBars = ssVertical
    TabOrder = 7
  end
  object SystemSettings: TGroupBox
    Left = 8
    Top = 8
    Width = 353
    Height = 233
    Caption = 'Настройки'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
    object FileOpenImg: TImage
      Left = 296
      Top = 24
      Width = 25
      Height = 21
      Stretch = True
      Transparent = True
    end
    object InitBKIImg: TImage
      Left = 120
      Top = 64
      Width = 25
      Height = 21
      Stretch = True
      Transparent = True
    end
    object InitBKSImg: TImage
      Left = 120
      Top = 104
      Width = 25
      Height = 21
      Stretch = True
      Transparent = True
    end
    object COMPortBKI: TComboBox
      Left = 8
      Top = 64
      Width = 105
      Height = 21
      ItemHeight = 13
      TabOrder = 0
      Text = 'COM-порт БКИ'
      OnChange = COMPortBKISelect
      OnDropDown = COMPortBKIDrop
    end
    object DeviceFilePath: TEdit
      Left = 8
      Top = 24
      Width = 201
      Height = 21
      TabOrder = 1
      Text = 'Выберите файл режима'
    end
    object DeviceFileButt: TButton
      Left = 214
      Top = 24
      Width = 75
      Height = 21
      Caption = 'Обзор...'
      TabOrder = 2
      OnClick = DeviceFileButtClick
    end
    object COMPortBKS: TComboBox
      Left = 8
      Top = 104
      Width = 105
      Height = 21
      ItemHeight = 13
      TabOrder = 3
      Text = 'COM-порт БКС'
      OnChange = COMPortBKSSelect
      OnDropDown = COMPortBKSDrop
    end
    object TimerBox: TCheckBox
      Left = 8
      Top = 136
      Width = 145
      Height = 17
      Caption = 'Сигналы без задержек'
      ParentShowHint = False
      ShowHint = False
      TabOrder = 4
    end
    object RoundBox: TCheckBox
      Left = 8
      Top = 168
      Width = 145
      Height = 17
      Caption = 'Циклическая проверка'
      TabOrder = 5
    end
    object ApplySets: TButton
      Left = 8
      Top = 200
      Width = 100
      Height = 25
      Caption = 'Применить'
      TabOrder = 6
      OnClick = ApplySetsClick
    end
    object DefaultSets: TButton
      Left = 120
      Top = 200
      Width = 100
      Height = 25
      Caption = 'Сбросить'
      TabOrder = 7
      OnClick = DefaultSetsClick
    end
    object MakeDeviceFile: TButton
      Left = 238
      Top = 200
      Width = 100
      Height = 25
      Caption = 'Создать файл'
      TabOrder = 8
      OnClick = MakeDeviceClick
    end
  end
  object SigDelayBKS: TTimer
    Enabled = False
    Interval = 100
    OnTimer = SigDelayBKSEvents
    Left = 504
    Top = 144
  end
  object SigDelayBKI: TTimer
    Enabled = False
    Interval = 100
    OnTimer = SigDelayBKIEvents
    Left = 552
    Top = 144
  end
  object DeviceFile: TOpenDialog
    Left = 184
    Top = 72
  end
end
