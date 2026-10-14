angkasatu = int(input("Tampilkan Angka: "))
operator = input("Masukkan operator anda: ")
angka2 = int(input("Ditambah angka kedua: "))

if operator == "+":
    hasil = angkasatu + angka2
elif operator == "*":
    hasil = angkasatu * angka2
elif operator == "/":
    hasil = angkasatu / angka2
elif operator == "-":
    hasil = angkasatu - angka2
else:
    print("Apasihhh")
    exit()

print("Angkanya adalah : ", hasil)