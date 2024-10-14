import random
if __name__ == "__main__":
    random.seed()
    my_rand = random.randint(0, 31)
    if my_rand < 0:
        print("my_rand < 0")
    elif my_rand >= 32:
        print("my_rand >= 32")
    else:
        print("0 <= my_rand < 32")