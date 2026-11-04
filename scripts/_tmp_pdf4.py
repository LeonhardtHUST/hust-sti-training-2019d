import pymupdf

doc = pymupdf.open(r'd:\Projects\GitHub\hust-sti-training-2019d\docs\XPulse\原理图 & 封装\原理图 STM32G474VET6.pdf')
page = doc[0]
clip = pymupdf.Rect(150, 230, 340, 320)
pix = page.get_pixmap(matrix=pymupdf.Matrix(8, 8), clip=clip)
pix.save(r'd:\Projects\GitHub\hust-sti-training-2019d\scripts\_tmp_mcupins.png')
print('saved', pix.width, pix.height)
