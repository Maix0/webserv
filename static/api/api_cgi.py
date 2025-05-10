#!/usr/bin/env python3

import json
import hashlib
from os import getenv
import sqlite3
import sys
import traceback

SECRET = "this is a very secret string oooooh"


def make_hash(s: str):
    return hashlib.sha1((s + SECRET).encode("utf-8")).hexdigest()


def login(db: sqlite3.Cursor, method: str, path: str):
    name = input()
    passwd = input()
    h = make_hash(passwd)

    res = db.execute(
        "SELECT session FROM users WHERE passwd = ? AND user = ?", (h, name)
    ).fetchone()
    if res is None:
        return "status: 403 Forbidden\r\n\r\nInvalid name or password\r\n"
    return (
        "status: 200 OK\r\n"
        f"set-cookie: session={res[0]}\r\n"
        "\r\n"
        f"Successfully logged in as {name}"
    )


def logout(db: sqlite3.Cursor, method: str, path: str):
    return (
        "status: 200 OK\r\n"
        "set-cookie: session=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"
        "\r\n"
        "Successfully logged out"
    )


def get_info(db: sqlite3.Cursor, method: str, path: str):
    c = getenv("HTTP_COOKIE")
    if c is None:
        return "status: 403 Forbidden\r\n\r\nNot Logged in\r\n"
    cookies = dict(map(lambda s: s.split("="), c.split(";")))
    if "session" not in cookies:
        return "status: 403 Forbidden\r\n\r\nNot Logged in\r\n"
    res = db.execute(
        "SELECT * FROM users WHERE session = ?", (cookies["session"],)
    ).fetchone()
    if res is None:
        return (
            "status: 403 Forbidden\r\n"
            "set-cookie: session=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"
            "\r\n"
            "Not Logged in\r\n"
        )
    return f"status: 200 OK\r\n\r\n{res[3]}\r\n"
    ...


def set_info(db: sqlite3.Cursor, method: str, path: str):
    c = getenv("HTTP_COOKIE")
    if c is None:
        return "status: 403 Forbidden\r\n\r\nNot Logged in\r\n"
    cookies = dict(map(lambda s: s.split("="), c.split(";")))
    if "session" not in cookies:
        return "status: 403 Forbidden\r\n\r\nNot Logged in\r\n"
    res = db.execute(
        "SELECT * FROM users WHERE session = ?", (cookies["session"],)
    ).fetchone()
    if res is None:
        return (
            "status: 403 Forbidden\r\n"
            "set-cookie: session=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"
            "\r\n"
            "Not Logged in\r\n"
        )
    new_info = input()
    db.execute(
        "UPDATE users SET info = ? WHERE session = ?", (new_info, cookies["session"])
    )
    return "status: 200 OK\r\n\r\nUpdated info :)\r\n"


def signin(db: sqlite3.Cursor, method: str, path: str):
    name = input()
    passwd = input()

    if len(name) < 4 or len(passwd) < 4:
        return (
            "status: 403 Forbidden\r\n"
            "\r\n"
            "Invalid name or password (both must be 4 char at least)\r\n"
        )
    phash = make_hash(passwd)
    session = make_hash(name)

    if db.execute("SELECT * FROM users WHERE user = ?", (name,)).fetchone() is not None:
        return "status: 403 Forbidden\r\n\r\nInvalid name: Already exists\r\n"
    db.execute("INSERT INTO users VALUES (?, ?, ?, ?)", (name, phash, session, None))
    return (
        "status: 200 OK\r\n"
        f"set-cookie: session={session}\r\n"
        "\r\n"
        "Created new user :)\r\n"
    )


def whoami(db: sqlite3.Cursor, method: str, path: str):
    c = getenv("HTTP_COOKIE")
    if c is None:
        return "status: 403 Forbidden\r\n\r\nNot Logged in\r\n"
    cookies = dict(map(lambda s: s.split("="), c.split(";")))
    if "session" not in cookies:
        return "status: 403 Forbidden\r\n\r\nNot Logged in\r\n"
    res = db.execute(
        "SELECT * FROM users WHERE session = ?", (cookies["session"],)
    ).fetchone()
    if res is None:
        return (
            "status: 403 Forbidden\r\n"
            "set-cookie: session=deleted; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"
            "\r\n"
            "Not Logged in\r\n"
        )
    return f"status: 200 OK\r\n\r\n{res[0]}\r\n"


def dump_db(db: sqlite3.Cursor, method: str, path: str):
    out = "status: 200 OK\r\ncontent-type: application/json\r\n\r\n"

    ret = db.execute("SELECT * FROM users").fetchall()
    data = [{"users": u, "passwd": p, "session": s, "info": i} for (u, p, s, i) in ret]
    out += json.dumps(data)
    out += "\r\n"
    return out


def not_found(db: sqlite3.Cursor, method: str, path: str):
    return (
        f"status: 404 Not Found\r\n\r\nApi '{path}' not found for method {method}\r\n"
    )


req = getenv("PATH_INFO")
method = getenv("REQUEST_METHOD")

if req is None:
    body = "status: 500 Internal Server Error\r\n\r\nMissing PATH_INFO\r\n"
    print(body, end="")
    exit(0)
if method is None:
    body = "status: 500 Internal Server Error\r\n\r\nMissing REQUEST_METHOD\r\n"
    print(body, end="")
    exit(0)

call = not_found

if req == "/api/signin.py" and method == "POST":
    call = signin
elif req == "/api/login.py" and method == "POST":
    call = login
elif req == "/api/logout.py" and method == "GET":
    call = logout
elif req == "/api/getinfo.py" and method == "GET":
    call = get_info
elif req == "/api/setinfo.py" and method == "POST":
    call = set_info
elif req == "/api/dumpdb.py" and method == "GET":
    call = dump_db
elif req == "/api/whoami.py" and method == "GET":
    call = whoami
else:
    call = not_found
db = sqlite3.connect("../users.db")
db.execute(
    """
CREATE TABLE IF NOT EXISTS users (
    user TEXT NOT NULL,
    passwd TEXT NOT NULL,
    session TEXT,
    info TEXT
);
"""
)
db.commit()


try:
    body = call(db.cursor(), method, req)
    db.commit()
    print(body, end="")
except Exception as _:
    body = (
        f"status: 500 Internal Server Error\r\n\r\nFailed:\r\n{traceback.format_exc()}"
    )
    print(body, end="")
sys.stdout.flush()
