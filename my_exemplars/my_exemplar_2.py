import random
def simple_recursion(my_rand):
    if my_rand > 1:
        simple_recursion(my_rand // 2)
if __name__ == "__main__":
    random.seed()
    my_rand = random.randint(0, 2**31 - 1)
    if my_rand % 2 == 0:
        simple_recursion(my_rand)
    else:
        simple_recursion(my_rand - 1)