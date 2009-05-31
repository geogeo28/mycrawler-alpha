#ifndef MACROS_H
#define MACROS_H

#define SerializationVersion  QDataStream::Qt_4_5

#define MC_DATASTREAM_WRITE(buff, stream) \
  QByteArray buff; \
  QDataStream stream(&buff, QIODevice::WriteOnly); \
  stream.setVersion(SerializationVersion);

#define MC_DATASTREAM_READ(in, stream) \
  QDataStream stream(in); \
  stream.setVersion(SerializationVersion);

#endif // MACROS_H
