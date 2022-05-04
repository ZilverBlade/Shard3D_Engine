import os
import re

final = []


def iniRead(data):
    global final
    x = re.findall("\[*[A-Z]*\]", data)
    for i in range(len(x)):
        data = data.replace(x[i], "")
    data = data.replace("\n\n", "")
    final.append(data.split(" = "))

    returnSetting("Shadows")


def returnSetting(setting):
    global final

    for i in range(len(final)):
        if setting in final[0][i]:
            print(final[0][i])

    print(final)


if os.path.isfile("settings.ini"):
    text_file = open("settings.ini", "r")
    data = text_file.read()

    text_file.close()

    iniRead(data)

else:
    print("FILE NOT FOUND")
