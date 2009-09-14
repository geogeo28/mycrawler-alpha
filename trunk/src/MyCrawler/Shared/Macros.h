#ifndef MACROS_H
#define MACROS_H

#include <QByteArray>
#include <QBuffer>
#include <QIODevice>
#include <QDataStream>

#define SerializationVersion  QDataStream::Qt_4_5

#define MC_DATASTREAM_WRITE(buffer, byteArray, dataStream) \
  QByteArray byteArray; \
  QBuffer buffer(&byteArray); \
  buffer.open(QIODevice::WriteOnly); \
  QDataStream dataStream(&buffer); \
  dataStream.setVersion(SerializationVersion);

#define MC_DATASTREAM_READ(buffer, dataStream) \
  QDataStream dataStream(buffer); \
  dataStream.setVersion(SerializationVersion);

#endif // MACROS_H
