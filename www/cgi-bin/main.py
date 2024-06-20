import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: ./main.py ARG1 ARG2 ARG3 ... ARGN")
        sys.exit(1)

    args = sys.argv[1:]

    print("Arguments received:")
    for i, arg in enumerate(args, start=1):
        print(f"ARG{i}: {arg}")

if __name__ == "__main__":
    main()
