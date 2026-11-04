import pymupdf

doc = pymupdf.open(r'd:\Projects\GitHub\hust-sti-training-2019d\docs\XPulse\原理图 & 封装\原理图 STM32G474VET6.pdf')
page = doc[0]
print('page rect', page.rect)
words = page.get_text('words')
# find anchor '按键&LED'
anchor = [w for w in words if 'LED' in w[4] and '&' in w[4]]
print('anchor', anchor)
# cluster: print words in 3 columns of x range containing KEY1
hits = [w for w in words if w[4] in ('KEY1', 'KEY2', 'LED_R', 'LED_G', 'LED_B', 'SW', 'LED1', 'LED2', 'R1', 'R2', 'R3', 'R4', 'R5')]
for w in sorted(hits, key=lambda w: (round(w[1]), w[0])):
    print(f'{w[4]:8s} x0={w[0]:7.1f} y0={w[1]:7.1f} x1={w[2]:7.1f} y1={w[3]:7.1f}')
