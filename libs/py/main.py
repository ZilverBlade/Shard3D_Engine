import configparser

config = configparser.ConfigParser()
config.read("settings.ini")


def readSettingFromHeader(section, setting):
    return config[section][setting]


def writeSettingToHeader(section, setting, value):
    config[section][setting] = value
    with open("settings.ini", "w") as configfile:
        config.write(configfile)


def readAllSettingsFromHeader(section):
    return list(config.items(section))


def readAllHeaders():
    return config.sections()


def addHeader(section):
    config.add_section(section)
    with open("settings.ini", "w") as configfile:
        config.write(configfile)


def removeHeader(section):
    config.remove_section(section)
    with open("settings.ini", "w") as configfile:
        config.write(configfile)

