#include "settings_position_spread.h"

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

#include <QVBoxLayout>
#include "groupbox_position_spread.h"

SettingsPositionSpread* SettingsPositionSpread::m_Instance = 0;

SettingsPositionSpread::SettingsPositionSpread()
{
    this->setObjectName("SettingsPositionSpread");
}

void SettingsPositionSpread::Init(PositionSpread *positionSpread)
{
    if(m_ContextMenuUi == -1)
    {
        m_ContextMenuUi = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_GLOBAL,"Position Spread","radar_16.png");
        connect(TSContextMenu::instance(), &TSContextMenu::MenusInitialized, this, &SettingsPositionSpread::onMenusInitialized, Qt::AutoConnection);
        connect(TSContextMenu::instance(), &TSContextMenu::FireContextMenuEvent, this, &SettingsPositionSpread::onContextMenuEvent, Qt::AutoConnection);
    }

    connect(this, &SettingsPositionSpread::EnabledSet, positionSpread, &PositionSpread::setEnabled);
    connect(this, &SettingsPositionSpread::ValueChanged, positionSpread, &PositionSpread::setSpreadWidth);
    connect(this, &SettingsPositionSpread::ExpertModeEnabledSet, positionSpread, &PositionSpread::setExpertModeEnabled);
    connect(this, &SettingsPositionSpread::RegionHomeTabSet, positionSpread, &PositionSpread::setRegionHomeTab);
    connect(this, &SettingsPositionSpread::RegionWhisperSet, positionSpread, &PositionSpread::setRegionWhisper);
    connect(this, &SettingsPositionSpread::RegionOtherSet, positionSpread, &PositionSpread::setRegionOther);

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);

    positionSpread->setEnabled(cfg.value("stereo_position_spread_enabled",true).toBool());
    positionSpread->setSpreadWidth(cfg.value("stereo_position_spread_value",0.5f).toFloat());
    positionSpread->setExpertModeEnabled(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
    positionSpread->setRegionHomeTab(cfg.value("stereo_position_spread_region_home",1).toInt());
    positionSpread->setRegionWhisper(cfg.value("stereo_position_spread_region_whisper",2).toInt());
    positionSpread->setRegionOther(cfg.value("stereo_position_spread_region_other",0).toInt());

    mP_positionSpread = positionSpread;
}

void SettingsPositionSpread::onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
    Q_UNUSED(serverConnectionHandlerID);
    Q_UNUSED(selectedItemID);

    if (type == PLUGIN_MENU_TYPE_GLOBAL)
    {
        if (menuItemID == m_ContextMenuUi)
        {
            if (config)
                config.data()->activateWindow();
            else
            {
                auto p_config = new QDialog(TSHelpers::GetMainWindow());
                p_config->setAttribute( Qt::WA_DeleteOnClose );
                p_config->setWindowTitle(mP_positionSpread.data()->objectName());
                auto groupBox = new GroupBoxPositionSpread(p_config);

                auto layout = new QVBoxLayout;
                layout->addWidget(groupBox);
                p_config->setLayout(layout);

                //ConfigPositionSpread* p_config = new ConfigPositionSpread(TSHelpers::GetMainWindow());  //has delete on close attribute

                QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
                groupBox->UpdateEnabledSet(cfg.value("stereo_position_spread_enabled",true).toBool());
                groupBox->UpdateSpreadWidth(cfg.value("stereo_position_spread_value",0.5f).toFloat());
                groupBox->UpdateExpertEnabledSet(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
                groupBox->UpdateExpertHomeValueChanged(cfg.value("stereo_position_spread_region_home",1).toInt());
                groupBox->UpdateExpertWhisperValueChanged(cfg.value("stereo_position_spread_region_whisper",2).toInt());
                groupBox->UpdateExpertOtherValueChanged(cfg.value("stereo_position_spread_region_other",0).toInt());

                connect(groupBox, &GroupBoxPositionSpread::EnabledSet, this, &SettingsPositionSpread::EnabledSet);
                connect(groupBox, &GroupBoxPositionSpread::ValueChanged, this, &SettingsPositionSpread::ValueChanged);

                connect(groupBox, &GroupBoxPositionSpread::ExpertEnabledSet, this, &SettingsPositionSpread::ExpertModeEnabledSet);
                connect(groupBox, &GroupBoxPositionSpread::ExpertHomeValueChanged, this, &SettingsPositionSpread::RegionHomeTabSet);
                connect(groupBox, &GroupBoxPositionSpread::ExpertWhisperValueChanged, this, &SettingsPositionSpread::RegionWhisperSet);
                connect(groupBox, &GroupBoxPositionSpread::ExpertOtherValueChanged, this, &SettingsPositionSpread::RegionOtherSet);

                connect(p_config, &QDialog::finished, this, &SettingsPositionSpread::saveSettings);
                p_config->show();
                config = p_config;
            }
        }
    }
}

void SettingsPositionSpread::onMenusInitialized()
{
    if(m_ContextMenuUi == -1)
        TSLogging::Error(QString("%1: Menu wasn't registered.").arg(this->objectName()));
}

void SettingsPositionSpread::saveSettings(int r)
{
    Q_UNUSED(r);
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("stereo_position_spread_enabled",mP_positionSpread.data()->isEnabled());
    cfg.setValue("stereo_position_spread_value",mP_positionSpread.data()->getSpreadWidth());

    cfg.setValue("stereo_position_spread_expert_enabled",mP_positionSpread.data()->isExpertModeEnabled());
    cfg.setValue("stereo_position_spread_region_home",(int)(mP_positionSpread.data()->getRegionHomeTab()));
    cfg.setValue("stereo_position_spread_region_whisper",(int)(mP_positionSpread.data()->getRegionWhisper()));
    cfg.setValue("stereo_position_spread_region_other",(int)(mP_positionSpread.data()->getRegionOther()));
}
