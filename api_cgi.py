#!/usr/bin/env python3

import json
import hashlib
from os import getenv
import sys

DB = {}

SECRET = "this is a very secret string oooooh"


def open_db():
    global DB
    with open("./users.json") as f:
        DB = json.load(f)


def close_db():
    global DB
    with open("./users.json", "wt") as f:
        json.dump(DB, f, indent=4)


if True:
    req = getenv("PATH_INFO")
    if req is None:
        print("status: 500 Internal Server Error\r\n", end="")
        print("\r\n", end="")
        print("MISSING PATH_INFO !!!!!\r\n", end="")
        exit(0)
    method = getenv("REQUEST_METHOD")
    if method is None:
        print("status: 500 Internal Server Error\r\n", end="")
        print("\r\n", end="")
        print("MISSING REQUEST_METHOD !!!!!\r\n", end="")
        exit(0)

    if req == "/api/signin.api" and method == "POST":
        name = input()
        password = input()
        sessionid = hashlib.sha1((name + SECRET).encode("utf-8")).hexdigest()
        open_db()
        if sessionid in DB:
            print("status: 403 Forbidden\r\n", end="")
            print("\r\n", end="")
            print("user already exists\r\n", end="")
        else:
            DB[sessionid] = {
                "pass": hashlib.md5(password.encode("utf-8")).hexdigest(),
                "name": name,
                "info": "",
            }
            close_db()
            print("status: 200 OK\r\n", end="")
            print(f'set-cookie: SESSIONID="{sessionid}"\r\n', end="")
            print("\r\n", end="")
            print("successfully logged in\r\n", end="")
            exit(0)
    elif req == "/api/setinfo.api" and method == "POST":
        c = getenv("HTTP_COOKIE")
        if c is None:
            print("status: 403 Forbidden\r\n", end="")
            print("\r\n", end="")
            print("Not logged in1\r\n", end="")
            exit(0)
        cookies = dict(map(lambda s: s.split("="), c.split(";")))
        if "SESSIONID" not in cookies:
            print("status: 403 Forbidden\r\n", end="")
            print("\r\n", end="")
            print("Not logged in2\r\n", end="")
            exit(0)
        open_db()
        if cookies["SESSIONID"] not in DB:
            print("status: 403 Forbidden\r\n", end="")
            print(
                "set-cookie: SESSIONID=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n",
                end="",
            )
            print("\r\n", end="")
            print("Not logged in\r\n", end="")
            exit(0)
        DB[cookies["SESSIONID"]]["info"] = input()
        close_db()
        print("status: 200 OK\r\n", end="")
        print("\r\n", end="")
        print("successfully set info\r\n", end="")
        exit(0)
    elif req == "/api/getinfo.api" and method == "GET":
        c = getenv("HTTP_COOKIE")
        if c is None:
            print("status: 403 Forbidden\r\n", end="")
            print("\r\n", end="")
            print("Not logged in1\r\n", end="")
            exit(0)
        cookies = dict(map(lambda s: s.split("="), c.split(";")))
        if "SESSIONID" not in cookies:
            print("status: 403 Forbidden\r\n", end="")
            print("\r\n", end="")
            print("Not logged in2\r\n", end="")
            exit(0)
        open_db()
        print(cookies, file=sys.stderr)
        print(DB, file=sys.stderr)
        if cookies["SESSIONID"] not in DB:
            print("status: 403 Forbidden\r\n", end="")
            print(
                "set-cookie: SESSIONID=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n",
                end="",
            )
            print("\r\n", end="")
            print("Not logged in3\r\n", end="")
            exit(0)
        print("status: 200 OK\r\n", end="")
        print("\r\n", end="")
        print(f"{DB[cookies['SESSIONID']]['info']}\r\n", end="")
        exit(0)
    elif req == "/api/logout.api" and method == "GET":
        print("status: 200 OK\r\n", end="")
        print(
            "set-cookie: SESSIONID=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n",
            end="",
        )
        print("\r\n", end="")
        print("logged out\r\n", end="")
        exit(0)
    elif req == "/api/login.api" and method == "POST":
        name = input()
        password = input()

        sessionid = hashlib.sha1((name + SECRET).encode("utf-8")).hexdigest()
        open_db()
        close_db()
        if (
            sessionid in DB
            and DB[sessionid]["pass"]
            == hashlib.md5(password.encode("utf-8")).hexdigest()
        ):
            print("status: 200 OK\r\n", end="")
            print(f'set-cookie: SESSIONID="{sessionid}"\r\n', end="")
            print("\r\n", end="")
            print(f"logged in as {name}\r\n", end="")
        else:
            print("status: 403 Forbidden\r\n", end="")
            print(
                "set-cookie: SESSIONID=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n",
                end="",
            )
            print("\r\n", end="")
            print("wrong credentials\r\n", end="")
            exit(0)
    else:
        print("status: 500 Internal Server Error\r\n", end="")
        print("\r\n", end="")
        print(f"Wrong endpoint: {method} {req} \r\n", end="")
