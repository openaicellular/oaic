Search.setIndex({"docnames": ["a1mediator", "background_cloud", "background_oran_architecture", "background_srsRAN", "example_xapp_deployment", "index", "nextranslicing", "oaic_features", "oran_installation", "quickstart", "requirements", "setup5gnetwork", "srsRAN_additional_features", "srsRAN_installation", "xapp_deployment", "xapp_example"], "filenames": ["a1mediator.rst", "background_cloud.rst", "background_oran_architecture.rst", "background_srsRAN.rst", "example_xapp_deployment.rst", "index.rst", "nextranslicing.rst", "oaic_features.rst", "oran_installation.rst", "quickstart.rst", "requirements.rst", "setup5gnetwork.rst", "srsRAN_additional_features.rst", "srsRAN_installation.rst", "xapp_deployment.rst", "xapp_example.rst"], "titles": ["A1 Mediator Installation Guide", "Background on Docker, Kubernetes &amp; Helm", "O-RAN Architecture", "RAN Architecture", "KPIMON xApp Deployment", "Welcome to OAIC\u2019s documentation!", "NexRAN Slicing xApp Installation &amp; running in Kubernetes", "OAIC Features", "O-RAN Near-Real Time RIC Installation Guide", "Getting Started", "Hardware and Software Requirements", "Setup your own 5G Network", "Additional Features", "srsRAN with E2 Agent Installation Guide", "xApp Deployment - General Guidelines", "Getting started: Developing xapps"], "terms": {"The": [0, 1, 4, 5, 8, 11, 12, 13, 14], "ran": [0, 5, 7, 9, 11, 13, 15], "intellig": [0, 8, 15], "control": [0, 8, 9, 11, 15], "ric": [0, 4, 5, 6, 9, 10, 14, 15], "platform": [0, 1, 4, 8, 11, 15], "s": [0, 1, 6, 7, 8, 11, 12, 13, 15], "compon": [0, 5, 8, 9, 11], "listen": [0, 14, 15], "request": [0, 4, 14, 15], "sent": [0, 12, 15], "via": [0, 8, 12, 15], "http": [0, 4, 6, 8, 9, 10, 13, 14, 15], "northbound": 0, "interfac": [0, 5, 8, 12, 13], "publish": [0, 15], "those": [0, 1, 8], "run": [0, 1, 4, 5, 8, 9, 12, 14, 15], "xapp": [0, 9, 11], "rmr": [0, 6, 8, 15], "messag": [0, 8, 11, 15], "southbound": 0, "To": [0, 1, 8, 9, 12, 14, 15], "get": [0, 1, 5, 6, 8, 11, 12, 14], "clone": [0, 8, 9, 13, 15], "github": [0, 1, 9, 10, 15], "com": [0, 6, 8, 9, 10, 15], "openaicellular": [0, 9, 10], "plt": [0, 8, 10, 15], "There": [0, 4, 8, 14], "ar": [0, 1, 4, 8, 9, 11, 12, 14, 15], "two": [0, 8, 11, 13, 15], "object": [0, 8], "associ": [0, 12], "defin": [0, 1, 8, 14], "name": [0, 1, 4, 6, 8, 11, 12, 14, 15], "descript": [0, 15], "most": [0, 8], "importantli": 0, "schema": [0, 15], "all": [0, 1, 4, 8, 9, 11, 14, 15], "think": 0, "json": [0, 1, 4, 6, 14, 15], "do": [0, 1, 8, 11, 12, 13, 14, 15], "receiv": [0, 15], "us": [0, 1, 4, 5, 8, 9, 11, 14], "onli": [0, 1, 8, 13], "valid": [0, 8], "creation": [0, 8], "howev": 0, "must": [0, 12, 14], "regist": [0, 8, 15], "id": [0, 6, 11, 15], "descriptor": [0, 4, 14, 15], "develop": [0, 9], "can": [0, 1, 4, 7, 8, 11, 12, 13, 14, 15], "also": [0, 1, 8, 9, 11, 12, 14, 15], "creat": [0, 1, 5, 6, 9, 11, 12, 13, 15], "new": [0, 1, 8, 11, 13, 15], "though": 0, "exact": 0, "process": [0, 1, 8, 9, 11], "where": [0, 1, 14], "store": [0, 1, 8, 14], "still": [0, 8, 12], "tbd": 0, "For": [0, 1, 8, 9, 11, 13, 15], "practic": 0, "purpos": [0, 9, 15], "when": [0, 1, 8, 11, 14], "need": [0, 1, 4, 8, 11, 12, 13, 14, 15], "invok": [0, 15], "load": [0, 8, 12], "befor": [0, 4, 8, 11, 12, 13], "sign": [0, 8], "up": [0, 1, 4, 8, 11, 12], "multipl": [0, 1, 5, 7, 8, 11, 13], "concret": 0, "instanti": 0, "thei": [0, 1, 8], "give": [0, 8], "valu": [0, 1, 8, 11, 15], "mai": [0, 1], "mani": [0, 1], "singl": [0, 1, 8, 11, 13], "whenev": 0, "over": [0, 8, 15], "see": [0, 4, 8, 11, 12, 13, 14], "below": [0, 8, 9, 12, 14, 15], "expect": 0, "handl": 0, "simultan": [0, 1], "each": [0, 1, 8, 12], "thi": [0, 4, 8, 9, 11, 12, 13, 14, 15], "list": [0, 1, 5, 6, 11, 15], "some": [0, 4, 8, 14], "between": [0, 1, 8, 12, 13], "here": [0, 4, 8, 11, 12, 13, 14], "date": 0, "2019": 0, "09": 0, "30": 0, "In": [0, 8, 11, 12], "case": [0, 1, 8, 12, 14, 15], "defici": 0, "ahead": 0, "other": [0, 1, 8, 11, 12, 15], "doe": [0, 8, 12], "yet": [0, 1], "conform": 0, "recent": 0, "chang": [0, 1, 8, 11, 12, 15], "notion": 0, "aspect": 0, "quit": 0, "critic": 0, "intend": 0, "implement": [0, 13, 15], "same": [0, 8, 11], "often": 0, "moreov": 0, "without": [0, 8], "cannot": [0, 1, 12], "whether": [0, 11], "which": [0, 1, 8, 9, 12, 13, 14, 15], "a1m": 0, "thing": [0, 14], "set": [0, 1, 8, 12, 15], "call": [0, 1, 8, 15], "one": [0, 1, 4, 8, 11, 12, 13, 14], "more": [0, 1, 8, 9, 15], "against": 0, "current": [0, 1, 4, 11, 12, 14, 15], "provid": [0, 1, 4, 8, 9, 15], "mechan": 0, "know": 0, "correct": [0, 8], "sinc": [0, 1, 8, 11], "them": [0, 4, 8, 14], "ha": [0, 1, 4, 8, 11, 12, 14, 15], "rather": 0, "larg": 0, "consequ": 0, "none": [0, 11, 15], "url": [0, 6, 14, 15], "match": 0, "rich": [0, 15], "statu": [0, 14, 15], "state": [0, 1, 8, 11, 14], "machin": [0, 1, 4, 11, 12, 13, 14], "actual": 0, "delet": [0, 4, 14], "point": [0, 4, 8, 14, 15], "referenc": 0, "404": 0, "configur": [0, 1, 5, 11, 13, 15], "option": [0, 11, 12, 13, 15], "deploi": [0, 1, 11, 14, 15], "cr": 0, "come": [0, 7, 9, 13], "patch": [0, 8], "partial": 0, "updat": [0, 9, 11, 12, 13], "team": 0, "agre": 0, "remov": [0, 1, 12], "later": 0, "version": [0, 1, 6, 8, 9, 12, 13, 14, 15], "have": [0, 1, 4, 8, 12, 13, 14], "oper": [0, 1, 10, 13, 15], "put": [0, 6], "bodi": 0, "exactli": 0, "scope": [0, 9], "statement": 0, "block": [0, 9, 12], "veri": [0, 8, 13], "close": 0, "otherwis": [0, 4, 14], "i": [0, 4, 6, 11, 12], "would": [0, 15], "argu": 0, "redund": 0, "exampl": [0, 1, 4, 8, 11, 14, 15], "kei": [0, 1, 8], "insid": [0, 8, 12], "an": [0, 1, 4, 5, 9, 11, 14, 15], "alreadi": [0, 4, 8, 11, 12, 13], "notifi": 0, "extern": [0, 5, 8, 15], "client": [0, 11, 15], "status": 0, "queri": [0, 15], "should": [0, 4, 8, 11, 12, 14], "return": [0, 15], "full": [0, 13, 15], "right": 0, "now": [0, 4, 8, 11, 12, 14, 15], "assum": [0, 8, 11, 12], "subsequ": 0, "fetch": 0, "document": [0, 13, 15], "detail": [0, 8, 13], "specif": [0, 8, 13], "allow": [0, 1, 8, 12], "section": [0, 4, 8, 11, 14, 15], "4": [0, 5, 6, 15], "2": [0, 6, 11, 12], "6": [0, 8], "we": [0, 1, 4, 8, 9, 11, 12, 13, 14, 15], "believ": 0, "so": [0, 1, 8, 11, 12], "quickli": 0, "becom": [0, 1, 5], "stale": 0, "major": [0, 8, 10], "failur": 0, "solut": 0, "sdl": [0, 15], "librari": [0, 1, 9], "persist": [0, 1, 8, 15], "inform": [0, 1, 12, 13, 15], "includ": [0, 1, 8, 11, 13], "If": [0, 4, 8, 11, 12, 14, 15], "built": [0, 14], "fail": [0, 15], "restart": [0, 1, 8, 11, 15], "lost": 0, "tini": 0, "bit": 0, "volatil": 0, "its": [0, 1, 8, 11], "next": [0, 4, 8, 14], "second": [0, 1, 11], "job": [0, 8], "queue": [0, 8], "memori": 0, "thread": 0, "poll": 0, "everi": [0, 9], "dequeu": 0, "perform": [0, 5, 8, 9, 14, 15], "were": 0, "kill": [0, 11], "time": [0, 5, 9, 10, 12, 15], "you": [0, 1, 4, 8, 9, 11, 12, 13, 14], "could": [0, 14], "mean": [0, 8], "wouldn": 0, "t": [0, 4, 6, 8, 9, 11, 13, 14], "take": [0, 8], "isn": 0, "drastic": 0, "idempot": 0, "alwai": [0, 8], "re": [0, 12], "order": [0, 8], "consid": 0, "complet": [0, 4, 8], "move": [0, 15], "redi": 0, "backend": [0, 15], "nativ": [0, 8, 13], "support": [0, 5, 8, 12, 13], "lpush": 0, "rpop": 0, "ve": [0, 8, 9], "ask": 0, "extens": 0, "follow": [0, 4, 8, 9, 11, 12, 14, 15], "launch": 0, "behavior": 0, "a1_rmr_retry_tim": 0, "number": [0, 1], "timeout": 0, "send": [0, 1, 8, 15], "retri": 0, "503": 0, "default": [0, 8, 11, 12, 14, 15], "instance_delete_no_resp_ttl": 0, "pleas": [0, 4, 8, 9, 12, 13], "refer": [0, 11, 15], "flowchart": 0, "doc": [0, 8, 15], "t1": 0, "5": [0, 8, 13], "basic": [0, 1, 8, 12, 15], "wait": 0, "databas": [0, 8, 11, 15], "after": [0, 8, 15], "downstream": 0, "app": [0, 1, 4, 6, 8, 14, 15], "respond": [0, 8], "instance_delete_resp_ttl": 0, "t2": 0, "use_fake_sdl": 0, "test": [0, 9, 12, 13, 15], "featur": [0, 1, 5, 13], "dbaa": 0, "fals": [0, 8], "prometheus_multiproc_dir": 0, "directori": [0, 4, 9, 11, 13, 14], "prometheu": [0, 8, 15], "gather": 0, "metric": [0, 12, 15], "tmp": 0, "offici": 0, "helm": [0, 5, 10, 14, 15], "chart": [0, 1, 6, 8, 14, 15], "repositori": [0, 1, 6, 8, 9, 12, 13, 14, 15], "hold": 0, "integration_test": 0, "integr": [0, 7, 9, 11, 13], "discuss": 0, "abov": [0, 8, 12, 15], "docker": [0, 5, 6, 10, 15], "cli": [0, 5, 15], "cach": 0, "latest": [0, 8, 9, 15], "depend": [0, 1, 9, 12, 15], "companion": [0, 5, 13], "conveni": 0, "shown": [0, 8], "mock": 0, "servic": [0, 4, 6, 8, 11, 14, 15], "sampl": [0, 13], "rout": [0, 8, 12, 15], "tabl": [0, 12, 13], "suppli": 0, "file": [0, 1, 5, 6, 8, 9, 11, 13, 15], "rt": [0, 4, 7, 15], "mount": 0, "volum": [0, 8], "command": [0, 1, 11, 12, 15], "both": [0, 1, 8, 9, 11], "e": [0, 7, 8, 10, 15], "true": [0, 6, 8, 11, 15], "p": [0, 6, 8, 13], "10000": 0, "v": [0, 8, 9], "path": [0, 15], "opt": 0, "A": [0, 1, 4, 8, 12, 14], "web": [0, 4], "user": [0, 1, 8, 9, 13], "gener": [0, 5, 8, 11, 15], "openapi": 0, "access": [0, 1, 4, 7, 8, 9, 12, 14], "host": [0, 1, 5, 11, 13], "ip": [0, 4, 6, 8, 11, 12, 14, 15], "ui": 0, "intern": [0, 11], "server": [0, 1, 4, 5, 8, 11, 15], "error": [0, 12, 14, 15], "storag": [0, 1, 8, 15], "data": [0, 4, 6, 8, 11, 12, 14, 15], "layer": 0, "avail": [0, 8, 15], "been": [0, 4, 14], "curl": [0, 4, 6, 14, 15], "healthcheck": 0, "instruct": [0, 1, 4, 8, 11, 12, 13, 15], "visit": 0, "o": [0, 4, 5, 6, 7, 9, 11, 13, 14, 15], "sc": [0, 8, 10, 15], "org": [0, 8, 15], "project": [0, 8, 9, 13, 15], "en": [0, 5, 8, 15], "index": [0, 5, 15], "html": [0, 8, 15], "manag": [1, 8, 15], "side": 1, "isol": 1, "environ": [1, 8, 9, 15], "everyth": 1, "applic": [1, 4, 6, 8, 13, 14, 15], "better": [1, 9], "comput": [1, 10], "densiti": 1, "reli": 1, "what": [1, 14], "instal": [1, 4, 9, 14], "secur": [1, 8], "given": [1, 4, 8, 12, 14], "differ": [1, 8, 9, 11, 13, 15], "virtual": [1, 12, 13], "wai": [1, 11], "os": [1, 8], "workload": [1, 8], "instanc": [1, 15], "With": 1, "vm": [1, 5, 11, 13], "hardwar": [1, 5, 8], "being": 1, "build": [1, 4, 6, 9, 13, 14, 15], "agil": 1, "softwar": [1, 5, 7, 9, 13], "deliveri": 1, "pipelin": 1, "ship": 1, "faster": 1, "repeat": 1, "linux": [1, 8, 15], "window": [1, 8, 11, 12], "read": [1, 8, 15], "templat": 1, "base": [1, 7, 10, 11, 12, 15], "custom": [1, 8, 14], "execut": [1, 8, 11, 12], "sourc": [1, 5, 12, 15], "code": [1, 5, 6, 8], "well": [1, 9], "tool": [1, 5, 8], "encapsul": 1, "runnabl": 1, "lightweight": 1, "standalon": 1, "packag": [1, 8, 12, 15], "runtim": [1, 15], "system": [1, 7, 12, 13, 15], "resourc": [1, 8, 15], "unless": 1, "addit": [1, 5], "variabl": [1, 14], "within": [1, 8, 9, 14], "specifi": [1, 8, 11], "much": [1, 15], "smaller": 1, "than": 1, "spun": 1, "matter": 1, "result": [1, 4], "ani": [1, 8, 14], "disappear": 1, "download": [1, 4, 6, 14, 15], "either": [1, 8], "local": [1, 4, 6, 8, 12, 14, 15], "public": 1, "like": [1, 9], "hub": 1, "similar": [1, 8, 12], "collabor": [1, 9], "push": [1, 6, 8, 14], "pull": [1, 8], "from": [1, 4, 6, 8, 11, 14, 15], "requir": [1, 5, 9, 12, 13], "text": 1, "how": [1, 5, 8, 15], "underli": [1, 8], "along": 1, "languag": [1, 15], "environment": 1, "locat": [1, 4, 14, 15], "network": [1, 4, 5, 8, 9, 12, 13], "port": [1, 12, 15], "onc": [1, 4, 8, 11, 12, 14, 15], "open": [1, 8, 9, 13], "autom": 1, "deploy": [1, 9, 10], "scale": 1, "across": [1, 8], "cluster": [1, 15], "centric": 1, "infrastructur": [1, 9], "It": [1, 11], "enabl": [1, 6, 8, 9, 12, 13, 14], "auto": 1, "placement": [1, 8], "replic": [1, 8], "might": 1, "cloud": [1, 8, 14], "aw": 1, "ec2": 1, "physic": [1, 12], "pod": [1, 4, 8, 14, 15], "equival": 1, "yum": 1, "apt": [1, 8, 12, 13, 14], "seen": [1, 8], "pip": [1, 12], "python": [1, 12], "just": [1, 8], "similarli": 1, "understand": [1, 8, 9], "yaml": [1, 8, 15], "anoth": [1, 8], "markup": 1, "help": [1, 8, 9], "type": [1, 4, 6, 11, 14, 15], "format": [1, 12], "label": 1, "metadata": [1, 4, 6, 8, 11, 14], "info": [1, 9, 15], "link": [1, 6, 9, 11, 12, 15], "fire": 1, "bunch": 1, "constitut": 1, "tiller": [1, 8], "go": [4, 5, 8, 14], "step": [4, 9, 12, 14], "proceed": 4, "As": 4, "indic": [4, 8, 11, 12, 14], "genral": [4, 11], "guidelin": [4, 5, 11], "first": [4, 8, 9, 11, 12, 14, 15], "continu": 4, "copi": [4, 6, 14], "reload": [4, 14], "nginx": 4, "done": [4, 8, 12, 14, 15], "sudo": [4, 6, 8, 11, 12, 13, 14], "cp": [4, 14], "scp": [4, 14], "var": [4, 14, 15], "www": [4, 8, 9, 14], "xapp_config": [4, 14], "config_fil": [4, 14], "systemctl": [4, 14], "newli": [4, 8, 14], "place": [4, 14], "want": [4, 11, 14], "export": [4, 6, 11, 13, 14, 15], "machine_ip": 4, "hostnam": [4, 8, 11], "cut": [4, 11], "f1": [4, 11], "d": [4, 6, 8, 11, 14, 15], "5010": [4, 6, 14], "make": [4, 8, 11, 12, 13, 14, 15], "possibl": 4, "cd": [4, 8, 9, 13, 14, 15], "registri": [4, 8, 14], "5008": [4, 14], "1": [4, 5, 6, 10, 11, 12, 14, 15], "0": [4, 6, 8, 11, 13, 14, 15], "essenti": [4, 5, 8, 13, 14], "5g": [4, 5, 7, 9, 10, 13], "subscript": 4, "procedur": 4, "sucess": 4, "readi": [4, 14, 15], "repres": [4, 8], "address": [4, 8, 11, 12, 14], "kong_proxi": [4, 6, 14], "kubectl": [4, 6, 8, 11, 14, 15], "svc": [4, 6, 8, 11, 14, 15], "n": [4, 6, 8, 10, 11, 14, 15], "ricplt": [4, 6, 8, 11, 14, 15], "l": [4, 6, 8, 14], "kubernet": [4, 5, 10, 14, 15], "io": [4, 6, 8, 14], "kong": [4, 6, 14], "jsonpath": [4, 6, 8, 11, 14], "item": [4, 6, 8, 11, 14], "spec": [4, 6, 8, 11, 14], "clusterip": [4, 6, 8, 11, 14, 15], "appmgr_http": [4, 6, 14], "field": [4, 6, 8, 11, 14], "selector": [4, 6, 8, 11, 14], "appmgr": [4, 6, 14, 15], "onboarder_http": [4, 6, 14], "good": [4, 14], "exist": [4, 8, 9, 14], "proce": [4, 14, 15], "forward": [4, 12, 14], "32080": [4, 6, 14, 15], "api": [4, 6, 8, 14, 15], "v1": [4, 6, 8, 14, 15], "ngnix": [4, 14], "vim": [4, 14], "past": [4, 14], "substitu": [4, 14], "machine_ip_addr": [4, 14], "your": [4, 5, 8, 12, 13, 14], "find": [4, 11, 14], "out": [4, 8, 9, 14], "through": [4, 8, 11, 14], "ifconfig": [4, 11, 12, 14], "json_url": [4, 6, 14], "save": [4, 12, 14], "x": [4, 6, 10, 12, 14], "post": [4, 6, 8, 14, 15], "header": [4, 6, 14, 15], "content": [4, 6, 11, 14, 15], "binari": [4, 6, 8, 13, 14, 15], "raw": [4, 6, 14, 15], "xappnam": [4, 6, 14, 15], "ricxapp": [4, 6, 14, 15], "namespac": [4, 8, 11, 14, 15], "log": [4, 6, 8, 11, 14, 15], "f": [4, 6, 8, 14, 15], "start": [5, 8, 11, 13], "introduct": [5, 8], "structur": [5, 15], "contributor": 5, "research": 5, "paper": 5, "mail": 5, "Near": [5, 9, 10], "real": [5, 9, 10], "cellular": [5, 9, 10, 13], "stack": [5, 8, 10, 13], "core": [5, 11, 13], "architectur": [5, 9, 15], "guid": [5, 15], "pre": [5, 12], "requisit": 5, "srsran": [5, 7, 9, 10, 11, 12], "e2": [5, 7, 10, 11, 12, 15], "agent": [5, 6, 7, 11], "zeromq": [5, 7, 12], "uhd": 5, "asn1c": 5, "compil": 5, "setup": [5, 12, 13], "own": [5, 8], "epc": [5, 12], "gnb": [5, 13], "ue": [5, 6, 7, 13], "mode": 5, "simul": [5, 7, 11, 13], "gnu": [5, 7, 11, 13], "radio": [5, 7, 9, 10, 11, 13], "connect": [5, 8], "internet": 5, "usrp": [5, 7, 10, 13], "virtualbox": 5, "enhanc": 5, "tip": 5, "trick": 5, "cot": 5, "kpimon": [5, 14], "config": [5, 6, 11, 15], "imag": [5, 12, 15], "onboard": [5, 6], "dm": [5, 15], "overview": [5, 8], "frame": 5, "common": [5, 8], "function": [5, 8], "framework": [5, 8, 9], "variou": 5, "hw": 5, "dms_cli": 5, "modul": [5, 8], "search": 5, "page": [5, 13], "e2mgr_http": 6, "e2mgr": [6, 8], "e2term_sctp": 6, "e2term": [6, 8, 11], "sctp": [6, 8, 11], "alpha": [6, 8, 11], "rtmgr_http": 6, "rtmgr": 6, "dockerfil": [6, 8, 14, 15], "8080": [6, 15], "tag": [6, 8, 14], "localhost": [6, 8, 11], "sh": [6, 8, 12, 13], "powder": [6, 13], "cdn": 6, "123": 6, "anonfil": 6, "rfn702bdu5": 6, "6ebc2dd4": 6, "1628643253": 6, "per": [6, 8], "line": [6, 15], "91": 6, "xget": 6, "xpost": 6, "home": [6, 11], "pratheek": 6, "kumar": 6, "srslte": [6, 13], "srsepc": [6, 11, 12, 13], "src": 6, "spgw": 6, "sgi_if_addr": 6, "192": [6, 12], "168": [6, 12], "srsenb": [6, 12, 13], "enb": [6, 11, 12, 13], "n_prb": [6, 11], "15": [6, 8, 11], "enb1": [6, 11], "enb_id": [6, 11], "0x19b": [6, 11], "rf": [6, 11], "device_nam": [6, 11], "zmq": [6, 11], "device_arg": [6, 11], "fail_on_disconnect": [6, 11], "base_sr": [6, 11], "23": [6, 11], "04e6": [6, 11], "tx_port": 6, "tcp": [6, 11, 12, 15], "2000": [6, 11], "rx_port": 6, "2001": [6, 11], "remote_ipv4_addr": [6, 11], "all_level": [6, 11], "warn": [6, 11], "log_level": [6, 11], "debug": [6, 11, 13], "filenam": [6, 11], "stdout": [6, 11], "slicer": 6, "workshar": 6, "netn": [6, 11], "add": [6, 11, 12, 13], "ue1": [6, 11], "srsue": [6, 12, 13], "usim": 6, "algo": 6, "xor": 6, "imsi": [6, 11], "001010123456789": 6, "k": 6, "00112233445566778899aabbccddeeff": 6, "imei": 6, "353490069873310": 6, "gw": [6, 12, 13], "nexran_xapp": 6, "echo": 6, "8000": [6, 11], "exec": [6, 11], "iperf": [6, 11], "36000": 6, "c": [6, 8, 9, 11, 12], "bin": [6, 8], "sleepint": 6, "nodeb": 6, "sleep": 6, "h": 6, "411": 6, "mcc": 6, "001": 6, "mnc": 6, "01": [6, 13], "fast": 6, "allocation_polici": 6, "proport": 6, "share": [6, 8], "1024": 6, "slow": 6, "256": 6, "bind": [6, 13], "enb_macro_001_001_0019b0": 6, "001010123456788": 6, "invert": 6, "prioriti": 6, "commun": [7, 8, 9, 11], "compliant": 7, "releas": [7, 8, 10, 11, 12], "nsa": [7, 11, 13], "interact": [7, 15], "osc": [7, 8], "ettu": [7, 13], "cornet": 7, "testb": 7, "soon": [7, 9, 13], "kpi": 7, "monitor": 7, "20": [8, 10, 13], "04": [8, 10, 13], "lt": 8, "bionic": 8, "beaver": 8, "cpu": [8, 13], "vcpu": 8, "ram": 8, "16": [8, 11, 12], "gb": 8, "160": 8, "prefer": 8, "recommend": [8, 15], "power": 8, "enough": 8, "10": [8, 10, 12, 13], "check": [8, 9, 11, 12, 14, 15], "video": [8, 9], "begin": [8, 13], "parent": [8, 13], "oaic": [8, 13], "outlin": [8, 11, 13], "realtim": 8, "spread": 8, "onto": 8, "separ": [8, 11], "auxiliari": 8, "aux": [8, 15], "respect": [8, 14], "consist": [8, 15], "kube": 8, "ns": [8, 15], "ricinfra": 8, "mainten": 8, "contain": [8, 12], "script": [8, 12], "demo": 8, "k8": [8, 15], "simpl": [8, 14, 15], "node": 8, "master": [8, 10], "slave": 8, "cover": [8, 15], "automat": 8, "paramet": 8, "privat": 8, "etc": [8, 14, 15], "infra": 8, "rc": 8, "cni": 8, "left": [8, 15], "unspecifi": 8, "env": 8, "normal": 8, "special": 8, "self": 8, "certif": 8, "resolv": 8, "entri": [8, 15], "openstack": 8, "relev": 8, "flexibl": 8, "our": [8, 9, 14], "ever": 8, "output": [8, 12, 15], "shell": 8, "1node": 8, "init": [8, 9], "k_1_16": 8, "h_2_17": 8, "d_cur": 8, "v2": [8, 15], "17": 8, "gen": 8, "internetwork": 8, "replac": 8, "reboot": 8, "upon": 8, "success": [8, 11, 12], "approx": 8, "min": 8, "back": [8, 15], "total": [8, 15], "9": [8, 13], "These": 8, "serv": 8, "briefli": 8, "coredn": 8, "dn": [8, 12], "flannel": 8, "overlai": 8, "work": [8, 15], "assign": [8, 11, 12], "rang": 8, "subnet": 8, "usual": [8, 12], "ipv4": 8, "facilit": [8, 9, 15], "inter": 8, "traffic": [8, 12], "transport": 8, "etcd": 8, "maintain": 8, "map": [8, 15], "alloc": 8, "try": 8, "highli": 8, "dictionari": 8, "apiserv": 8, "plane": 8, "expos": 8, "front": 8, "end": 8, "accordingli": 8, "proxi": 8, "iptabl": 8, "rule": [8, 15], "static": 8, "endpoint": 8, "balanc": 8, "goe": 8, "down": 8, "remain": 8, "schedul": 8, "watch": [8, 9, 15], "select": 8, "determin": 8, "accord": 8, "constraint": 8, "collect": [8, 15], "polici": [8, 15], "interfer": 8, "deadlin": 8, "respons": [8, 11], "notic": 8, "off": 8, "task": 8, "portion": 8, "typic": 8, "discontinu": 8, "v3": 8, "wa": 8, "risk": 8, "But": [8, 11], "persistentvolum": 8, "class": 8, "doesn": 8, "three": [8, 15], "nf": 8, "stabl": 8, "provision": 8, "storageclass": 8, "annot": 8, "resid": 8, "area": 8, "super": 8, "permiss": 8, "5001": 8, "5000": 8, "image_nam": 8, "image_tag": 8, "navig": 8, "repo": [8, 15], "adapt": [8, 12], "vespa": 8, "smo": 8, "exchang": 8, "accept": 8, "note": 8, "nodeport": 8, "while": [8, 11, 12], "view": [8, 15], "recipe_exampl": 8, "example_recipe_oran_e_release_modified_e2": 8, "recip": 8, "import": [8, 11, 12, 15], "concept": 8, "group": 8, "site": [8, 14], "bronz": 8, "cherri": 8, "dawn": 8, "benefit": 8, "seamless": 8, "again": [8, 11], "suit": [8, 13], "instead": 8, "previou": [8, 12, 14], "present": 8, "privilegedmod": 8, "hostnetworkmod": 8, "print": 8, "messagecollectorfil": 8, "outgo": 8, "datavols": 8, "100mi": 8, "storageclassnam": 8, "pizpub": 8, "click": [8, 12], "undeploy": 8, "correspond": 8, "submodul": [8, 9], "rest": [8, 15], "non": 8, "readm": [8, 15], "md": [8, 15], "youtub": [8, 9], "x5mhydijwmc": 8, "dep": [8, 15], "engin": 8, "educ": 8, "aquasec": 8, "academi": 8, "digitalocean": 8, "tutori": [8, 12, 13], "7": 8, "velotio": 8, "blog": 8, "fabric": 8, "8": [8, 12], "sookocheff": 8, "model": 8, "administr": 8, "effort": 9, "led": 9, "consortium": 9, "academ": 9, "institut": 9, "fulli": 9, "toolset": 9, "encompass": 9, "ai": 9, "spur": 9, "leverag": 9, "extend": [9, 13], "industri": 9, "standard": [9, 15], "futur": [9, 15], "rst": 9, "lmui8hqkdyg": 9, "315": 9, "r9gcbwslzjw": 9, "1s": 9, "dwhrvluswaq": 9, "git": [9, 13, 15], "root": [9, 14], "recurs": 9, "remot": 9, "gbp": 10, "nic": [10, 12], "sdr": [10, 11, 13], "g": 10, "x310": 10, "n310": 10, "ubuntu": [10, 13], "oran": 10, "tree": 10, "e_rel_xapp_onboarder_support": 10, "termin": [10, 11], "a1": [10, 15], "mediat": 10, "suffic": 11, "sure": [11, 12, 13, 14, 15], "found": [11, 12, 15], "e2node_ip": 11, "e2node_port": 11, "5006": 11, "e2term_ip": 11, "50": 11, "tx_port0": 11, "rx_port0": 11, "tx_port1": 11, "2100": 11, "rx_port1": 11, "2101": 11, "local_ipv4_addr": 11, "local_port": 11, "successfulli": [11, 12, 15], "consol": 11, "initi": [11, 15], "establish": 11, "rrc": [11, 12], "nr": 11, "reconfigur": 11, "confirm": 11, "cell": 11, "lte": 11, "relat": 11, "user_db": 11, "csv": 11, "modif": 11, "exit": 11, "simplest": 11, "space": 11, "172": [11, 12], "ue_ip": 11, "issu": 11, "scenario": 11, "udp": 11, "10mbp": 11, "60": 11, "b": 11, "10m": 11, "x300": 11, "clock": 11, "sampling_r": 11, "11": 11, "52e6": 11, "lo_freq_offset_hz": 11, "send_frame_s": 11, "recv_frame_s": 11, "num_send_fram": 11, "64": 11, "num_recv_fram": 11, "input": [11, 12], "srate": 11, "rat": 11, "band": [11, 12], "3": [11, 13], "nof_carri": 11, "youhav": 11, "particular": 11, "ad": 11, "addr": 11, "ip_aadress_of_usrp": 11, "argument": [11, 12], "directli": [11, 15], "ue_ip_addr": 11, "grc": 12, "manipul": 12, "visual": 12, "q": 12, "compat": 12, "transmit": 12, "broker": 12, "act": 12, "channel": 12, "flow": 12, "uninstal": 12, "gnuradio": 12, "ppa": [12, 13], "python3": 12, "increas": 12, "volk_profil": 12, "encount": 12, "masquerad": 12, "srsepc_if_masq": 12, "translat": 12, "pass": 12, "boot": 12, "ref": 12, "identifi": 12, "kernel": 12, "destin": 12, "gatewai": 12, "genmask": 12, "flag": 12, "ifac": 12, "ug": 12, "600": 12, "wlp2s0": 12, "255": 12, "u": 12, "100": [12, 15], "enp0s3": 12, "1000": 12, "masq": 12, "wifi": 12, "plug": 12, "obtain": 12, "On": 12, "tun_srsu": 12, "ping": 12, "sgi": 12, "That": 12, "googl": 12, "10g": 12, "usb3": 12, "fpga": 12, "secondari": 12, "tab": 12, "adpat": 12, "leav": 12, "alon": 12, "attach": [12, 15], "choos": 12, "bridg": 12, "direct": 12, "Then": 12, "visibl": 12, "enp0s8": 12, "24": 12, "dev": [12, 13, 15], "uhd_find_devic": 12, "abl": 12, "4g": 13, "sr": 13, "modifi": 13, "21": 13, "sa": 13, "enodeb": 13, "light": 13, "weight": 13, "mme": 13, "hss": 13, "licens": [13, 15], "ghz": 13, "frontend": 13, "At": 13, "least": 13, "cmake": 13, "libfftw3": 13, "libmbedtl": 13, "libboost": 13, "program": 13, "libconfig": 13, "libsctp": 13, "libtool": 13, "autoconf": 13, "transfer": 13, "approach": 13, "ci": 13, "teach": 13, "demonstr": [13, 15], "look": 13, "libzmq3": 13, "don": 13, "manual": 13, "ettusresearch": 13, "libuhd": 13, "libuhd4": 13, "cn": 13, "air": 13, "oai": 13, "gitlab": 13, "eurecom": 13, "fr": 13, "checkout": [13, 15], "velichkov_s1ap_plus_option_group": 13, "autoreconf": 13, "iv": 13, "j": 13, "nproc": 13, "ldconfig": 13, "mkdir": [13, 14], "realpath": 13, "dcmake_build_typ": 13, "relwithdebinfo": 13, "dric_generated_e2ap_binding_dir": 13, "e2_bind": 13, "e2ap": 13, "v01": 13, "dric_generated_e2sm_kpm_binding_dir": 13, "e2sm": 13, "kpm": 13, "dric_generated_e2sm_gnb_nrt_binding_dir": 13, "nrt": 13, "srsran_install_config": 13, "forc": 13, "webserv": 14, "activ": 14, "unlink": 14, "conf": 14, "default_serv": 14, "server_nam": 14, "path_to_config_fil": 14, "uniqu": 14, "xappkpimon": 14, "keep": 14, "too": 14, "edit": 14, "previous": 14, "verifi": 14, "describ": 15, "write": 15, "writer": 15, "wiki": 15, "17269011": 15, "xapp_writer_s_guide_v2": 15, "pdf": 15, "modificationd": 15, "1625642899082": 15, "easier": 15, "construct": 15, "sever": 15, "skeleton": 15, "design": 15, "rapid": 15, "figur": 15, "depict": 15, "high": 15, "level": 15, "loos": 15, "scratch": 15, "rapidli": 15, "fledg": 15, "illustr": 15, "prerequisit": 15, "inject": 15, "dynam": 15, "filter": 15, "popul": 15, "mdclog": 15, "trace": 15, "asn": 15, "decod": 15, "encod": 15, "health": 15, "probe": 15, "db": 15, "rnib": 15, "uenib": 15, "statist": 15, "met": 15, "scrap": 15, "counter": 15, "gerrit": 15, "r": 15, "admin": 15, "written": 15, "aim": 15, "rais": 15, "alarm": 15, "intereact": 15, "method": 15, "show": 15, "hwapp": 15, "main": 15, "pkg": 15, "struct": 15, "a1_policy_queri": 15, "20013": 15, "policy_query_payload": 15, "policy_type_id": 15, "20000": 15, "func": 15, "sendpolicyqueri": 15, "logger": 15, "prepar": 15, "rmrparam": 15, "mtype": 15, "payload": 15, "byte": 15, "flg": 15, "sendmsg": 15, "els": 15, "configchangehandl": 15, "string": 15, "xappstartcb": 15, "consum": 15, "msg": 15, "err": 15, "getricmessagenam": 15, "meid": 15, "subid": 15, "txid": 15, "len": 15, "rannam": 15, "xid": 15, "payloadlen": 15, "switch": 15, "handler": 15, "a1_policy_request": 15, "reciv": 15, "ric_health_check_req": 15, "unknown": 15, "discard": 15, "defer": 15, "free": 15, "mbuf": 15, "nil": 15, "mdc": 15, "setmdc": 15, "addconfigchangelisten": 15, "callback": 15, "setreadycb": 15, "waitforsdl": 15, "getbool": 15, "runwithparam": 15, "optiuon": 15, "mirror": 15, "last": 15, "offer": 15, "util": 15, "chartmusem": 15, "folder": 15, "txt": 15, "uta_rtg": 15, "mod": 15, "sum": 15, "helm3": 15, "One": 15, "availab": 15, "proceur": 15, "chart_repo_url": 15, "fine": 15, "config_file_path": 15, "shcema_file_path": 15, "httpget": 15, "readinessprob": 15, "tojson": 15, "initialdelaysecond": 15, "periodsecond": 15, "livenessprob": 15, "onbord": 15, "apivers": 15, "appvers": 15, "tgz": 15, "2021": 15, "06": 15, "24t18": 15, "57": 15, "41": 15, "98056196z": 15, "digest": 15, "14a484d9a394ed34eab66e5241ec33e73be8fa70a2107579d19d037f2adf57a0": 15, "wish": 15, "ver": 15, "helmvers": 15, "null": 15, "po": 15, "ag": 15, "55ff7549df": 15, "kpj6k": 15, "2m": 15, "ip1": 15, "80": 15, "443": 15, "73d": 15, "ip2": 15, "103m": 15, "ip3": 15, "4560": 15, "4561": 15, "jq": 15, "xferd": 15, "averag": 15, "speed": 15, "dload": 15, "upload": 15, "spent": 15, "347": 15, "578": 15, "579": 15, "rxmessag": 15, "ric_sub_resp": 15, "a1_policy_req": 15, "txmessag": 15, "ric_sub_req": 15, "a1_policy_resp": 15, "ric_health_check_resp": 15}, "objects": {}, "objtypes": {}, "objnames": {}, "titleterms": {"a1": 0, "mediat": 0, "instal": [0, 5, 6, 8, 12, 13, 15], "guid": [0, 8, 13], "overview": [0, 15], "code": [0, 9, 15], "polici": 0, "type": 0, "instanc": 0, "known": 0, "differ": 0, "from": [0, 12, 13], "1": [0, 8, 13], "0": [0, 12], "spec": 0, "resili": 0, "environ": 0, "variabl": [0, 4], "kubernet": [0, 1, 6, 8], "deploy": [0, 4, 5, 8, 14, 15], "local": 0, "build": [0, 8], "imag": [0, 1, 4, 8, 14], "start": [0, 9, 15], "contain": [0, 1], "view": 0, "api": 0, "check": [0, 4], "health": 0, "background": [1, 5], "docker": [1, 4, 8, 14], "helm": [1, 4, 8], "core": 1, "compon": [1, 10, 15], "registri": 1, "dockerfil": 1, "o": [2, 8, 10], "ran": [2, 3, 8, 10], "architectur": [2, 3, 8], "kpimon": 4, "xapp": [4, 5, 6, 7, 14, 15], "host": [4, 8, 14], "config": [4, 14], "file": [4, 14], "creat": [4, 8, 14], "onboard": [4, 14, 15], "get": [4, 9, 13, 15], "chart": 4, "url": 4, "deploi": [4, 8], "verifi": 4, "welcom": 5, "oaic": [5, 7, 9], "s": 5, "document": 5, "inform": 5, "procedur": [5, 6, 8], "develop": [5, 13, 15], "indic": 5, "tabl": 5, "nexran": 6, "slice": 6, "run": [6, 11], "srsran": [6, 13], "featur": [7, 12], "Near": [7, 8], "real": [7, 8], "time": [7, 8], "ric": [7, 8, 11], "cellular": 7, "stack": 7, "support": [7, 15], "pre": [8, 13, 14], "requisit": [8, 13, 14], "system": [8, 10], "requir": [8, 10], "softwar": [8, 10, 15], "step": 8, "configur": [8, 12, 14], "an": [8, 12], "ubuntu": 8, "machin": 8, "virtual": 8, "vm": [8, 12], "2": 8, "cluster": 8, "onetim": 8, "setup": [8, 11], "influxdb": 8, "3": [8, 12], "modifi": 8, "e2": [8, 13], "command": 8, "relat": 8, "termin": 8, "4": [8, 13], "structur": [8, 9], "directori": 8, "refer": 8, "introduct": 9, "how": 9, "becom": 9, "contributor": 9, "sourc": [9, 13], "research": 9, "paper": 9, "mail": 9, "list": 9, "hardwar": 10, "integr": 10, "solut": 10, "each": 10, "ue": [10, 11, 12], "os": 10, "packag": [10, 13], "framework": [10, 15], "your": 11, "own": 11, "5g": 11, "network": 11, "epc": 11, "en": 11, "gnb": 11, "zeromq": [11, 13], "mode": 11, "connect": [11, 12], "rt": 11, "srsenb": 11, "srsue": 11, "exchang": 11, "traffic": 11, "ping": 11, "uplink": 11, "downlink": 11, "iperf3": 11, "side": 11, "uhd": [11, 12, 13], "usrp": [11, 12], "x310": [11, 12], "test": 11, "addit": 12, "multipl": 12, "simul": 12, "us": [12, 13, 15], "gnu": 12, "radio": 12, "companion": 12, "binari": 12, "extern": 12, "server": [12, 14], "internet": 12, "virtualbox": 12, "b210": 12, "b": 12, "205": 12, "mini": 12, "over": 12, "usb": 12, "n310": 12, "through": 12, "ethernet": 12, "todo": [12, 13], "perform": 12, "enhanc": 12, "tip": 12, "trick": 12, "cot": 12, "agent": 13, "depend": 13, "librari": 13, "manag": 13, "asn1c": 13, "compil": 13, "gener": 14, "guidelin": 14, "web": 14, "nginx": 14, "dm": 14, "cli": 14, "The": 15, "frame": 15, "common": 15, "function": 15, "interfac": 15, "go": 15, "variou": 15, "essenti": 15, "hello": 15, "world": 15, "hw": 15, "dms_cli": 15, "tool": 15}, "envversion": {"sphinx.domains.c": 2, "sphinx.domains.changeset": 1, "sphinx.domains.citation": 1, "sphinx.domains.cpp": 6, "sphinx.domains.index": 1, "sphinx.domains.javascript": 2, "sphinx.domains.math": 2, "sphinx.domains.python": 3, "sphinx.domains.rst": 2, "sphinx.domains.std": 2, "sphinx": 56}})