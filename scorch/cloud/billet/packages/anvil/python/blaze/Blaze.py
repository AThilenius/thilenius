#
# Autogenerated by Thrift Compiler (0.9.2)
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#
#  options string: py
#

from thrift.Thrift import TType, TMessageType, TException, TApplicationException
from ttypes import *
from thrift.Thrift import TProcessor
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol, TProtocol
try:
  from thrift.protocol import fastbinary
except:
  fastbinary = None


class Iface:
  def ProcessBlazeRequest(self, token, command_name, args_json):
    """
    Parameters:
     - token
     - command_name
     - args_json
    """
    pass

  def GetMinecraftAccount(self, token):
    """
    Parameters:
     - token
    """
    pass

  def SetMinecraftAccount(self, token, minecraft_account):
    """
    Parameters:
     - token
     - minecraft_account
    """
    pass


class Client(Iface):
  def __init__(self, iprot, oprot=None):
    self._iprot = self._oprot = iprot
    if oprot is not None:
      self._oprot = oprot
    self._seqid = 0

  def ProcessBlazeRequest(self, token, command_name, args_json):
    """
    Parameters:
     - token
     - command_name
     - args_json
    """
    self.send_ProcessBlazeRequest(token, command_name, args_json)
    return self.recv_ProcessBlazeRequest()

  def send_ProcessBlazeRequest(self, token, command_name, args_json):
    self._oprot.writeMessageBegin('ProcessBlazeRequest', TMessageType.CALL, self._seqid)
    args = ProcessBlazeRequest_args()
    args.token = token
    args.command_name = command_name
    args.args_json = args_json
    args.write(self._oprot)
    self._oprot.writeMessageEnd()
    self._oprot.trans.flush()

  def recv_ProcessBlazeRequest(self):
    iprot = self._iprot
    (fname, mtype, rseqid) = iprot.readMessageBegin()
    if mtype == TMessageType.EXCEPTION:
      x = TApplicationException()
      x.read(iprot)
      iprot.readMessageEnd()
      raise x
    result = ProcessBlazeRequest_result()
    result.read(iprot)
    iprot.readMessageEnd()
    if result.success is not None:
      return result.success
    if result.operation_failure is not None:
      raise result.operation_failure
    raise TApplicationException(TApplicationException.MISSING_RESULT, "ProcessBlazeRequest failed: unknown result");

  def GetMinecraftAccount(self, token):
    """
    Parameters:
     - token
    """
    self.send_GetMinecraftAccount(token)
    return self.recv_GetMinecraftAccount()

  def send_GetMinecraftAccount(self, token):
    self._oprot.writeMessageBegin('GetMinecraftAccount', TMessageType.CALL, self._seqid)
    args = GetMinecraftAccount_args()
    args.token = token
    args.write(self._oprot)
    self._oprot.writeMessageEnd()
    self._oprot.trans.flush()

  def recv_GetMinecraftAccount(self):
    iprot = self._iprot
    (fname, mtype, rseqid) = iprot.readMessageBegin()
    if mtype == TMessageType.EXCEPTION:
      x = TApplicationException()
      x.read(iprot)
      iprot.readMessageEnd()
      raise x
    result = GetMinecraftAccount_result()
    result.read(iprot)
    iprot.readMessageEnd()
    if result.success is not None:
      return result.success
    if result.operation_failure is not None:
      raise result.operation_failure
    raise TApplicationException(TApplicationException.MISSING_RESULT, "GetMinecraftAccount failed: unknown result");

  def SetMinecraftAccount(self, token, minecraft_account):
    """
    Parameters:
     - token
     - minecraft_account
    """
    self.send_SetMinecraftAccount(token, minecraft_account)
    self.recv_SetMinecraftAccount()

  def send_SetMinecraftAccount(self, token, minecraft_account):
    self._oprot.writeMessageBegin('SetMinecraftAccount', TMessageType.CALL, self._seqid)
    args = SetMinecraftAccount_args()
    args.token = token
    args.minecraft_account = minecraft_account
    args.write(self._oprot)
    self._oprot.writeMessageEnd()
    self._oprot.trans.flush()

  def recv_SetMinecraftAccount(self):
    iprot = self._iprot
    (fname, mtype, rseqid) = iprot.readMessageBegin()
    if mtype == TMessageType.EXCEPTION:
      x = TApplicationException()
      x.read(iprot)
      iprot.readMessageEnd()
      raise x
    result = SetMinecraftAccount_result()
    result.read(iprot)
    iprot.readMessageEnd()
    if result.operation_failure is not None:
      raise result.operation_failure
    return


class Processor(Iface, TProcessor):
  def __init__(self, handler):
    self._handler = handler
    self._processMap = {}
    self._processMap["ProcessBlazeRequest"] = Processor.process_ProcessBlazeRequest
    self._processMap["GetMinecraftAccount"] = Processor.process_GetMinecraftAccount
    self._processMap["SetMinecraftAccount"] = Processor.process_SetMinecraftAccount

  def process(self, iprot, oprot):
    (name, type, seqid) = iprot.readMessageBegin()
    if name not in self._processMap:
      iprot.skip(TType.STRUCT)
      iprot.readMessageEnd()
      x = TApplicationException(TApplicationException.UNKNOWN_METHOD, 'Unknown function %s' % (name))
      oprot.writeMessageBegin(name, TMessageType.EXCEPTION, seqid)
      x.write(oprot)
      oprot.writeMessageEnd()
      oprot.trans.flush()
      return
    else:
      self._processMap[name](self, seqid, iprot, oprot)
    return True

  def process_ProcessBlazeRequest(self, seqid, iprot, oprot):
    args = ProcessBlazeRequest_args()
    args.read(iprot)
    iprot.readMessageEnd()
    result = ProcessBlazeRequest_result()
    try:
      result.success = self._handler.ProcessBlazeRequest(args.token, args.command_name, args.args_json)
    except OperationFailure, operation_failure:
      result.operation_failure = operation_failure
    oprot.writeMessageBegin("ProcessBlazeRequest", TMessageType.REPLY, seqid)
    result.write(oprot)
    oprot.writeMessageEnd()
    oprot.trans.flush()

  def process_GetMinecraftAccount(self, seqid, iprot, oprot):
    args = GetMinecraftAccount_args()
    args.read(iprot)
    iprot.readMessageEnd()
    result = GetMinecraftAccount_result()
    try:
      result.success = self._handler.GetMinecraftAccount(args.token)
    except OperationFailure, operation_failure:
      result.operation_failure = operation_failure
    oprot.writeMessageBegin("GetMinecraftAccount", TMessageType.REPLY, seqid)
    result.write(oprot)
    oprot.writeMessageEnd()
    oprot.trans.flush()

  def process_SetMinecraftAccount(self, seqid, iprot, oprot):
    args = SetMinecraftAccount_args()
    args.read(iprot)
    iprot.readMessageEnd()
    result = SetMinecraftAccount_result()
    try:
      self._handler.SetMinecraftAccount(args.token, args.minecraft_account)
    except OperationFailure, operation_failure:
      result.operation_failure = operation_failure
    oprot.writeMessageBegin("SetMinecraftAccount", TMessageType.REPLY, seqid)
    result.write(oprot)
    oprot.writeMessageEnd()
    oprot.trans.flush()


# HELPER FUNCTIONS AND STRUCTURES

class ProcessBlazeRequest_args:
  """
  Attributes:
   - token
   - command_name
   - args_json
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRUCT, 'token', (sentinel.ttypes.Token, sentinel.ttypes.Token.thrift_spec), None, ), # 1
    (2, TType.STRING, 'command_name', None, None, ), # 2
    (3, TType.LIST, 'args_json', (TType.STRING,None), None, ), # 3
  )

  def __init__(self, token=None, command_name=None, args_json=None,):
    self.token = token
    self.command_name = command_name
    self.args_json = args_json

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRUCT:
          self.token = sentinel.ttypes.Token()
          self.token.read(iprot)
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRING:
          self.command_name = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 3:
        if ftype == TType.LIST:
          self.args_json = []
          (_etype10, _size7) = iprot.readListBegin()
          for _i11 in xrange(_size7):
            _elem12 = iprot.readString();
            self.args_json.append(_elem12)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ProcessBlazeRequest_args')
    if self.token is not None:
      oprot.writeFieldBegin('token', TType.STRUCT, 1)
      self.token.write(oprot)
      oprot.writeFieldEnd()
    if self.command_name is not None:
      oprot.writeFieldBegin('command_name', TType.STRING, 2)
      oprot.writeString(self.command_name)
      oprot.writeFieldEnd()
    if self.args_json is not None:
      oprot.writeFieldBegin('args_json', TType.LIST, 3)
      oprot.writeListBegin(TType.STRING, len(self.args_json))
      for iter13 in self.args_json:
        oprot.writeString(iter13)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    return


  def __hash__(self):
    value = 17
    value = (value * 31) ^ hash(self.token)
    value = (value * 31) ^ hash(self.command_name)
    value = (value * 31) ^ hash(self.args_json)
    return value

  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ProcessBlazeRequest_result:
  """
  Attributes:
   - success
   - operation_failure
  """

  thrift_spec = (
    (0, TType.STRING, 'success', None, None, ), # 0
    (1, TType.STRUCT, 'operation_failure', (OperationFailure, OperationFailure.thrift_spec), None, ), # 1
  )

  def __init__(self, success=None, operation_failure=None,):
    self.success = success
    self.operation_failure = operation_failure

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 0:
        if ftype == TType.STRING:
          self.success = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 1:
        if ftype == TType.STRUCT:
          self.operation_failure = OperationFailure()
          self.operation_failure.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ProcessBlazeRequest_result')
    if self.success is not None:
      oprot.writeFieldBegin('success', TType.STRING, 0)
      oprot.writeString(self.success)
      oprot.writeFieldEnd()
    if self.operation_failure is not None:
      oprot.writeFieldBegin('operation_failure', TType.STRUCT, 1)
      self.operation_failure.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    return


  def __hash__(self):
    value = 17
    value = (value * 31) ^ hash(self.success)
    value = (value * 31) ^ hash(self.operation_failure)
    return value

  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class GetMinecraftAccount_args:
  """
  Attributes:
   - token
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRUCT, 'token', (sentinel.ttypes.Token, sentinel.ttypes.Token.thrift_spec), None, ), # 1
  )

  def __init__(self, token=None,):
    self.token = token

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRUCT:
          self.token = sentinel.ttypes.Token()
          self.token.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('GetMinecraftAccount_args')
    if self.token is not None:
      oprot.writeFieldBegin('token', TType.STRUCT, 1)
      self.token.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    return


  def __hash__(self):
    value = 17
    value = (value * 31) ^ hash(self.token)
    return value

  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class GetMinecraftAccount_result:
  """
  Attributes:
   - success
   - operation_failure
  """

  thrift_spec = (
    (0, TType.STRUCT, 'success', (MinecraftAccount, MinecraftAccount.thrift_spec), None, ), # 0
    (1, TType.STRUCT, 'operation_failure', (OperationFailure, OperationFailure.thrift_spec), None, ), # 1
  )

  def __init__(self, success=None, operation_failure=None,):
    self.success = success
    self.operation_failure = operation_failure

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 0:
        if ftype == TType.STRUCT:
          self.success = MinecraftAccount()
          self.success.read(iprot)
        else:
          iprot.skip(ftype)
      elif fid == 1:
        if ftype == TType.STRUCT:
          self.operation_failure = OperationFailure()
          self.operation_failure.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('GetMinecraftAccount_result')
    if self.success is not None:
      oprot.writeFieldBegin('success', TType.STRUCT, 0)
      self.success.write(oprot)
      oprot.writeFieldEnd()
    if self.operation_failure is not None:
      oprot.writeFieldBegin('operation_failure', TType.STRUCT, 1)
      self.operation_failure.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    return


  def __hash__(self):
    value = 17
    value = (value * 31) ^ hash(self.success)
    value = (value * 31) ^ hash(self.operation_failure)
    return value

  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class SetMinecraftAccount_args:
  """
  Attributes:
   - token
   - minecraft_account
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRUCT, 'token', (sentinel.ttypes.Token, sentinel.ttypes.Token.thrift_spec), None, ), # 1
    (2, TType.STRUCT, 'minecraft_account', (MinecraftAccount, MinecraftAccount.thrift_spec), None, ), # 2
  )

  def __init__(self, token=None, minecraft_account=None,):
    self.token = token
    self.minecraft_account = minecraft_account

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRUCT:
          self.token = sentinel.ttypes.Token()
          self.token.read(iprot)
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRUCT:
          self.minecraft_account = MinecraftAccount()
          self.minecraft_account.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('SetMinecraftAccount_args')
    if self.token is not None:
      oprot.writeFieldBegin('token', TType.STRUCT, 1)
      self.token.write(oprot)
      oprot.writeFieldEnd()
    if self.minecraft_account is not None:
      oprot.writeFieldBegin('minecraft_account', TType.STRUCT, 2)
      self.minecraft_account.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    return


  def __hash__(self):
    value = 17
    value = (value * 31) ^ hash(self.token)
    value = (value * 31) ^ hash(self.minecraft_account)
    return value

  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class SetMinecraftAccount_result:
  """
  Attributes:
   - operation_failure
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRUCT, 'operation_failure', (OperationFailure, OperationFailure.thrift_spec), None, ), # 1
  )

  def __init__(self, operation_failure=None,):
    self.operation_failure = operation_failure

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRUCT:
          self.operation_failure = OperationFailure()
          self.operation_failure.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('SetMinecraftAccount_result')
    if self.operation_failure is not None:
      oprot.writeFieldBegin('operation_failure', TType.STRUCT, 1)
      self.operation_failure.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    return


  def __hash__(self):
    value = 17
    value = (value * 31) ^ hash(self.operation_failure)
    return value

  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)
