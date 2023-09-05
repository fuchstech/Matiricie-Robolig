mac = str(input("Mac Address"))
macl = ""
for x in mac.split(":"):
    x = "0x"+x+","
    macl+=x
text = "uint8_t broadcastAddress[] = {" + macl.rstrip(",") + "};"
print(text)

