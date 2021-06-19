#include "OpenRGBSettingsPage.h"
#include "ui_OpenRGBSettingsPage.h"
#include "ResourceManager.h"

#ifdef _WIN32
#include "AutoStart-windows.h"
#endif

#ifdef __linux__
#include "AutoStart-linux.h"
#endif

using namespace Ui;

OpenRGBSettingsPage::OpenRGBSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBSettingsPageUi)
{
    ui->setupUi(this);

    /*---------------------------------------------------------*\
    | Load theme settings (Windows only)                        |
    \*---------------------------------------------------------*/
#ifdef _WIN32
    ui->ComboBoxTheme->addItems({"auto", "light", "dark"});

    json theme_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("Theme");

    if(theme_settings.contains("theme"))
    {
        std::string theme = theme_settings["theme"];
        ui->ComboBoxTheme->setCurrentText(QString::fromStdString(theme));
    }
    else
    {
        ui->ComboBoxTheme->setCurrentText(QString::fromStdString(("light")));
    }

    theme_initialized = true;
#else
    ui->ComboBoxTheme->hide();
    ui->ThemeLabel->hide();
#endif

    /*---------------------------------------------------------*\
    | Load user interface settings                              |
    \*---------------------------------------------------------*/
    json ui_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("UserInterface");

    if(ui_settings.contains("minimize_on_close"))
    {
        ui->CheckboxMinimizeOnClose->setChecked(ui_settings["minimize_on_close"]);
    }

    if(ui_settings.contains("geometry"))
    {
        if(ui_settings["geometry"].contains("load_geometry"))
        {
            ui->CheckboxLoadGeometry->setChecked(ui_settings["geometry"]["load_geometry"]);
        }

        if(ui_settings["geometry"].contains("save_on_exit"))
        {
            ui->CheckboxSaveGeometry->setChecked(ui_settings["geometry"]["save_on_exit"]);
        }
    }

#if defined(_WIN32) or defined(__linux__)
    /*---------------------------------------------------------*\
    | Load AutoStart settings                                   |
    \*---------------------------------------------------------*/

    // Populate profile Combo Box
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        ui->ComboBoxAutoStartProfile->clear();

        for(std::size_t profile_index = 0; profile_index < profile_manager->profile_list.size(); profile_index++)
        {
            ui->ComboBoxAutoStartProfile->addItem(profile_manager->profile_list[profile_index].c_str());
        }
    }

    // Populate level Combo Box
    ui->ComboBoxAutoStartLevel->addItems({"Critical", "Error", "Message", "Warning", "Notice", "Verbose", "Debug"});

    // Make sure autostart settings exist
    CreateAutoStartSettings();

    // Initialise UI to current settings or defaults
    json autostart_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("AutoStart");

    RemediateAutoStartProfile(autostart_settings);

    ui->ComboBoxAutoStartLevel->setCurrentText(QString::fromStdString(autostart_settings["level"]));
    ui->ComboBoxAutoStartLevel->setEnabled(autostart_settings["setlevel"]);

    // Text boxes
    ui->TextServerPort->setText(QString::fromStdString(autostart_settings["port"]));
    ui->TextServerPort->setEnabled(autostart_settings["setserverport"]);

    ui->TextClientHost->setText(QString::fromStdString(autostart_settings["client"]));
    ui->TextClientHost->setEnabled(autostart_settings["setclient"]);

    // Checkboxes
    ui->CheckboxAutoStart->setChecked(autostart_settings["enabled"]);
    SetAutoStartVisibility(autostart_settings["enabled"]);

    ui->CheckboxAutoStartMinimized->setChecked(autostart_settings["setminimized"]);
    ui->CheckboxAutoStartClient->setChecked(autostart_settings["setclient"]);
    ui->CheckboxAutoStartServer->setChecked(autostart_settings["setserver"]);
    ui->CheckboxAutoStartSetServerPort->setChecked(autostart_settings["setserverport"]);
    ui->CheckboxAutoStartLevel->setChecked(autostart_settings["setlevel"]);

    // Only show for errors
    ui->AutoStartStatusLabel->hide();
    autostart_initialized = true;
#else
    SetAutoStartVisibility(false);
    ui->CheckboxAutoStart->hide();
#endif
}

OpenRGBSettingsPage::~OpenRGBSettingsPage()
{
    delete ui;
}

// Theme slots
void OpenRGBSettingsPage::on_ComboBoxTheme_currentTextChanged(const QString theme)
{
    if(theme_initialized)
    {
        json theme_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("Theme");
        theme_settings["theme"] = theme.toStdString();
        ResourceManager::get()->GetSettingsManager()->SetSettings("Theme", theme_settings);
        SaveSettings();
    }
}

// UI slots
void OpenRGBSettingsPage::on_CheckboxMinimizeOnClose_clicked()
{
    json ui_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("UserInterface");
    ui_settings["minimize_on_close"] = ui->CheckboxMinimizeOnClose->isChecked();
    ResourceManager::get()->GetSettingsManager()->SetSettings("UserInterface", ui_settings);
    SaveSettings();
}

void Ui::OpenRGBSettingsPage::on_CheckboxLoadGeometry_clicked()
{
    json ui_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("UserInterface");
    ui_settings["geometry"]["load_geometry"] = ui->CheckboxLoadGeometry->isChecked();
    ResourceManager::get()->GetSettingsManager()->SetSettings("UserInterface", ui_settings);
    SaveSettings();
}

void Ui::OpenRGBSettingsPage::on_CheckboxSaveGeometry_clicked()
{
    json ui_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("UserInterface");
    ui_settings["geometry"]["save_on_exit"] = ui->CheckboxSaveGeometry->isChecked();
    ResourceManager::get()->GetSettingsManager()->SetSettings("UserInterface", ui_settings);
    SaveSettings();
}

// AutoStart slots
void Ui::OpenRGBSettingsPage::on_CheckboxAutoStart_clicked()
{
    if(autostart_initialized)
    {
        json autostart_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("AutoStart");
        autostart_settings["enabled"] = ui->CheckboxAutoStart->isChecked();
        if (autostart_settings["enabled"])
        {
            RemediateAutoStartProfile(autostart_settings);
        }
        ResourceManager::get()->GetSettingsManager()->SetSettings("AutoStart", autostart_settings);
        SaveSettings();

        SetAutoStartVisibility(autostart_settings["enabled"]);

        ConfigureAutoStart();
    }
}

void Ui::OpenRGBSettingsPage::on_CheckboxAutoStartMinimized_clicked()
{
    SaveAutoStartSetting("setminimized", ui->CheckboxAutoStartMinimized->isChecked());
}

void Ui::OpenRGBSettingsPage::on_CheckboxAutoStartServer_clicked()
{
    SaveAutoStartSetting("setserver", ui->CheckboxAutoStartServer->isChecked());
}

void Ui::OpenRGBSettingsPage::on_CheckboxAutoStartSetServerPort_clicked()
{
    SaveAutoStartSetting("setserverport", ui->CheckboxAutoStartSetServerPort->isChecked());
    ui->TextServerPort->setEnabled(ui->CheckboxAutoStartSetServerPort->isChecked());
}

void Ui::OpenRGBSettingsPage::on_CheckboxAutoStartClient_clicked()
{
    SaveAutoStartSetting("setclient", ui->CheckboxAutoStartClient->isChecked());
    ui->TextClientHost->setEnabled(ui->CheckboxAutoStartClient->isChecked());
}

void Ui::OpenRGBSettingsPage::on_CheckboxAutoStartProfile_clicked()
{
    SaveAutoStartSetting("setprofile", ui->CheckboxAutoStartProfile->isChecked());
    ui->ComboBoxAutoStartProfile->setEnabled(ui->CheckboxAutoStartProfile->isChecked());
}

void Ui::OpenRGBSettingsPage::on_CheckboxAutoStartLevel_clicked()
{
    SaveAutoStartSetting("setlevel", ui->CheckboxAutoStartLevel->isChecked());
    ui->ComboBoxAutoStartLevel->setEnabled(ui->CheckboxAutoStartLevel->isChecked());
}

void Ui::OpenRGBSettingsPage::on_TextServerPort_editingFinished()
{
    SaveAutoStartSetting("port", ui->TextServerPort->text());
}

void Ui::OpenRGBSettingsPage::on_TextClientHost_editingFinished()
{
    SaveAutoStartSetting("client", ui->TextClientHost->text());
}

void Ui::OpenRGBSettingsPage::on_ComboBoxAutoStartProfile_currentTextChanged(const QString profile)
{
    SaveAutoStartSetting("profile", profile);
}

void Ui::OpenRGBSettingsPage::on_ComboBoxAutoStartLevel_currentTextChanged(const QString level)
{
    SaveAutoStartSetting("level", level);
}

// AutoStart Helper Functions
void OpenRGBSettingsPage::SaveAutoStartSetting(std::string name, QString value)
{
    if(autostart_initialized)
    {
        json autostart_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("AutoStart");
        autostart_settings[name] = value.toStdString();
        ResourceManager::get()->GetSettingsManager()->SetSettings("AutoStart", autostart_settings);
        SaveSettings();

        ConfigureAutoStart();
    }
}

void OpenRGBSettingsPage::SaveAutoStartSetting(std::string name, bool value)
{
    if(autostart_initialized)
    {
        json autostart_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("AutoStart");
        autostart_settings[name] = value;
        ResourceManager::get()->GetSettingsManager()->SetSettings("AutoStart", autostart_settings);
        SaveSettings();

        ConfigureAutoStart();
    }
}

void OpenRGBSettingsPage::SetAutoStartVisibility(bool visible)
{
    if (!visible)
    {
        ui->ComboBoxAutoStartLevel->hide();
        ui->ComboBoxAutoStartProfile->hide();
        ui->CheckboxAutoStartClient->hide();
        ui->CheckboxAutoStartLevel->hide();
        ui->CheckboxAutoStartMinimized->hide();
        ui->CheckboxAutoStartProfile->hide();
        ui->CheckboxAutoStartServer->hide();
        ui->CheckboxAutoStartSetServerPort->hide();
        ui->TextClientHost->hide();
        ui->TextServerPort->hide();
        ui->AutoStartLabel->hide();
        ui->AutoStartStatusLabel->hide();
    }
    else
    {
        ui->ComboBoxAutoStartLevel->show();
        ui->ComboBoxAutoStartProfile->show();
        ui->CheckboxAutoStartClient->show();
        ui->CheckboxAutoStartLevel->show();
        ui->CheckboxAutoStartMinimized->show();
        ui->CheckboxAutoStartProfile->show();
        ui->CheckboxAutoStartServer->show();
        ui->CheckboxAutoStartSetServerPort->show();
        ui->TextClientHost->show();
        ui->TextServerPort->show();
        ui->AutoStartLabel->show();
        ui->AutoStartStatusLabel->show();
    }
}

void OpenRGBSettingsPage::ConfigureAutoStart()
{
    std::map<std::string, std::tuple<std::string, std::string, bool>> autostart_map = {
        {"setminimized", {"--startminimized","",false}},
        {"setserver", {"--server","",false}},
        {"setserverport", {"--server-port","port",false}},
        {"setclient", {"--client","client",false}},
        {"setprofile", {"--profile","profile",true}},
        {"setlevel", {"--loglevel","level",false}},
    };

    json autostart_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("AutoStart");

    AutoStart as("OpenRGB");

    if (!autostart_settings["enabled"])
    {
        if (!as.DisableAutoStart())
        {
            ui->AutoStartStatusLabel->setText("A problem occurred disabling Start At Login.");
            ui->AutoStartStatusLabel->show();
            SetAutoStartVisibility(true);
        }
        else
        {
            ui->AutoStartStatusLabel->hide();
        }
    }
    else
    {
        std::string desc = "OpenRGB ";
        desc += VERSION_STRING;
        desc += ", for controlling RGB lighting.";

        std::string arguments;

        for(std::pair<const std::string, std::tuple<std::string, std::string, bool>>& x: autostart_map)
        {
            std::string argumentsetting = x.first;
            std::string argument = std::get<0>(x.second);
            std::string argumentvaluename = std::get<1>(x.second);
            bool argumentquoted = std::get<2>(x.second);

            if (autostart_settings[x.first])
            {
                arguments += " " + argument;
                if (argumentvaluename != "")
                {
                    std::string argumentvalue = autostart_settings[argumentvaluename];
                    if (argumentquoted)
                    {
                        arguments += " \"" + argumentvalue + "\"";
                    }
                    else
                    {
                        arguments += " " + argumentvalue;
                    }
                }
            }
        }

        AutoStartInfo asi;
        asi.args = arguments;
        asi.category = "Utility;";
        asi.desc = desc;
        asi.icon = "OpenRGB";
        asi.path = as.GetExePath();

        if (!as.EnableAutoStart(asi))
        {
            ui->AutoStartStatusLabel->setText("A problem occurred enabling Start At Login.");
            ui->AutoStartStatusLabel->show();
            SetAutoStartVisibility(true);
        }
        else
        {
            ui->AutoStartStatusLabel->hide();
        }
    }
}

// Sets initial values for the AutoStart settings
void OpenRGBSettingsPage::CreateAutoStartSettings()
{
    std::map<std::string, std::string> autostart_default_map_string = {
        {"level", "Message"},
        {"port", "6742"},
        {"client","localhost:6742"},
        {"profile",ui->ComboBoxAutoStartProfile->count() > 0 ? ui->ComboBoxAutoStartProfile->itemText(0).toStdString(): ""}
    };

    std::map<std::string, bool> autostart_default_map_bool = {
        {"enabled", false},
        {"setminimized", false},
        {"setclient", false},
        {"setserver", false},
        {"setserverport", false},
        {"setlevel", false},
        {"setprofile", false},
    };

    json autostart_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("AutoStart");

    for(std::pair<const std::string, bool>& x: autostart_default_map_bool)
    {
        if(!autostart_settings.contains(x.first))
        {
            autostart_settings[x.first] = x.second;
        }
    }

    for(std::pair<const std::string, std::string>& x: autostart_default_map_string)
    {
        if(!autostart_settings.contains(x.first))
        {
            autostart_settings[x.first] = x.second;
        }
    }

    ResourceManager::get()->GetSettingsManager()->SetSettings("AutoStart", autostart_settings);
}

void OpenRGBSettingsPage::RemediateAutoStartProfile(json &autostart_settings)
{
    // If there are no profiles
    // Then we disable the UI for profiles
    // And if AutoStart is enabled
    // Then we force disable setprofile
    if(ui->ComboBoxAutoStartProfile->count() == 0)
    {
        ui->CheckboxAutoStartProfile->setEnabled(false);
        ui->ComboBoxAutoStartProfile->setEnabled(false);
        autostart_settings["profile"] = "";

        if (autostart_settings["enabled"])
        {
            autostart_settings["setprofile"] = false;
            ResourceManager::get()->GetSettingsManager()->SetSettings("AutoStart", autostart_settings);
            ConfigureAutoStart();
            SaveSettings();
        }
    }

    // Else If the profile we want doesn't exist
    // Then we force it to a profile which exists
    // And if AutoStart is enabled
    // Then we force disable setprofile
    else if (autostart_settings["profile"] == "" ||
            (autostart_settings["profile"] != "" &&
             ui->ComboBoxAutoStartProfile->findText(QString::fromStdString(autostart_settings["profile"])) == -1))
    {
        autostart_settings["profile"] = ui->ComboBoxAutoStartProfile->itemText(0).toStdString();
        if (autostart_settings["enabled"])
        {
            autostart_settings["setprofile"] = false;
            ResourceManager::get()->GetSettingsManager()->SetSettings("AutoStart", autostart_settings);
            ConfigureAutoStart();
            SaveSettings();
        }
    }

    ui->ComboBoxAutoStartProfile->setCurrentText(QString::fromStdString(autostart_settings["profile"]));
    ui->ComboBoxAutoStartProfile->setEnabled(autostart_settings["setprofile"]);
    ui->CheckboxAutoStartProfile->setChecked(autostart_settings["setprofile"]);
}

void OpenRGBSettingsPage::SaveSettings()
{
    ResourceManager::get()->GetSettingsManager()->SaveSettings();
}

