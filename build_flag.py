import json
import sys

def main(build_flag, json_key, quotes="true"):
    with open("build_config.json") as fp:
        parsed = json.load(fp)
        path = json_key.split(".")

        for key in path:
            parsed = parsed[key]
    
    if quotes == "true":
        print(f"-D{build_flag.upper()}=\\\"{parsed}\\\"")
    else:
        print(f"-D{build_flag.upper()}={parsed}")

if __name__ == "__main__":
    main(*sys.argv[1:])
