from datetime import datetime

# Aktuelles Datum und Uhrzeit erhalten
now = datetime.now()

# Formatieren des Datums und der Uhrzeit
formatted_now = now.strftime("%d.%m.%Y %H:%M:%S")

# Ausgabe des Formats
print(formatted_now)
