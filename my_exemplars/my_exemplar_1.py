def find_minimum(i, j):
    if i <= j:
        return i
    return j
if __name__ == "__main__":
    i = 6
    j = 2
    print(f"min({i}, {j}) = {find_minimum(i, j)}")