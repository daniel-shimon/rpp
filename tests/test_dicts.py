from tests.utils import Test, main

code = '''
גילאים = מילון()
לכל א בתוך טווח(300):
    גילאים[טקסט(א)] = אקראי()
לכל א בתוך טווח(300):
    גילאים[טקסט(א)] = גילאים[טקסט(א)] + 1
הדפס גילאים
'''

test = Test(code, [], validation_func=lambda out: out.count(':') == 300 and out.count(',') == 299)

if __name__ == '__main__':
    main(test)
