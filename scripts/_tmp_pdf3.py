import pymupdf

doc = pymupdf.open(r'd:\Projects\GitHub\hust-sti-training-2019d\docs\XPulse\原理图 & 封装\原理图 STM32G474VET6.pdf')
page = doc[0]
clip = pymupdf.Rect(440, 500, 640, 820)
pix = page.get_pixmap(matrix=pymupdf.Matrix(5, 5), clip=clip)
pix.save(r'd:\Projects\GitHub\hust-sti-training-2019d\scripts\_tmp_keyled.png')
print('saved', pix.width, pix.height)
print('--- words in clip ---')
for w in page.get_text('words', clip=clip):
    print(f'{w[4]:12s} x={w[0]:7.1f} y={w[1]:7.1f}')
