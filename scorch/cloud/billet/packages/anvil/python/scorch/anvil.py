from pprint import pprint
import getpass
import json
import os
import os.path
import sys

from blaze import Blaze
from blaze.ttypes import *
from blaze.ttypes import OperationFailure as BlazeOperationFailure
from sentinel import Sentinel
from sentinel.ttypes import *
from sentinel.ttypes import OperationFailure as SentinelOperationFailure
from thrift import Thrift
from thrift.transport import THttpClient
from thrift.protocol import TJSONProtocol

token_file_path = '.anvil/sentinel_key.json'
ip = os.popen("/sbin/ip route|awk \'/default/ { printf $3   }\'").read()

def get_token_():
    sentinel_transport = THttpClient.THttpClient(ip, 2100, '/')
    sentinel_protocol = TJSONProtocol.TJSONProtocol(sentinel_transport)
    sentinel = Sentinel.Client(sentinel_protocol)
    sentinel_transport.open()
    if os.path.isfile(token_file_path):
        with open(token_file_path) as token_file:
            token_json = json.load(token_file)
            token = Token()
            token.user_uuid = token_json['user_uuid']
            token.token_uuid = token_json['token_uuid']
            token.permission_level = token_json['permission_level']
            return token
    # Load the token from CIN
    token = sentinel.CreateToken(raw_input("Email Address: "),
            getpass.getpass("Password: "))
    if not os.path.exists(os.path.dirname(token_file_path)):
            os.makedirs(os.path.dirname(token_file_path))
    with open(token_file_path, 'w') as token_file:
        token_file.write(json.dumps(token, default=lambda o: o.__dict__))
    return token

token = get_token_();

def exec_blaze_(function):
    try:
        blaze_transport = THttpClient.THttpClient(ip, 2400, '/')
        blaze_protocol = TJSONProtocol.TJSONProtocol(blaze_transport)
        blaze = Blaze.Client(blaze_protocol)
        blaze_transport.open()
        ret_value = function(blaze)
        return ret_value
    except BlazeOperationFailure, op_failure:
        print 'Woops, Blaze is complaining that: %s' % (op_failure.user_message)
        exit()
    except Thrift.TException, tx:
        print 'TException: %s' % (tx.message)
        exit()
    finally:
        blaze_transport.close()

def set_minecraft_account_name_function_(name):
    def blaze_function(blaze):
        minecraft_account = MinecraftAccount()
        minecraft_account.minecraft_username = name;
        blaze.SetMinecraftAccount(token, minecraft_account)
    return blaze_function

def action_target_invoke_function_(world_id, command, args_obj):
    def blaze_function(blaze):
        return blaze.ProcessBlazeRequest(token, 'action_target_invoke',
                [str(world_id), command, json.dumps(args_obj)])
    return blaze_function

def set_minecraft_account(username):
    exec_blaze_(set_minecraft_account_name_function_(username))

def move_spark(world_id, direction):
    resText = exec_blaze_(action_target_invoke_function_(world_id, 'move',
        {'direction':direction}))
    return json.loads(resText)['did_pass']

def mine_spark(world_id, direction):
    resText = exec_blaze_(action_target_invoke_function_(world_id, 'mine',
        {'direction':direction}))
    return json.loads(resText)['did_pass']

def drop_items_spark(world_id):
    resText = exec_blaze_(action_target_invoke_function_(world_id, 'drop', {}))
    return json.loads(resText)['did_pass']

def recall_spark(world_id):
    resText = exec_blaze_(action_target_invoke_function_(world_id, 'recall', {}))
    return json.loads(resText)['did_pass']
