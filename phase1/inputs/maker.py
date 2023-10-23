# Open a file for writing
with open("file.txt", "w") as file:
    # The line you want to write
    line = "ASML 650 s#\nASML 645 b#\nAAPL 430 b#\nAAPL 435 b#\nAAPL 435 s#\nASML 646 b#\nASML 647 b#\nASML 651 b#\nAAPL 432 s#\nASML 654 s#\nASML 652 s#\nASML 650 s#\n"
    
    # Write the line 50 times
    for _ in range(50):
        file.write(line)