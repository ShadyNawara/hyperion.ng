#include "LedDeviceFile.h"

// Qt includes
#include <Qt>
#include <QTextStream>

LedDeviceFile::LedDeviceFile(const QJsonObject &deviceConfig)
	: LedDevice(deviceConfig)
	, _file (nullptr)
{
	_printTimeStamp = false;
}

LedDeviceFile::~LedDeviceFile()
{
	delete _file;
}

LedDevice* LedDeviceFile::construct(const QJsonObject &deviceConfig)
{
	return new LedDeviceFile(deviceConfig);
}

bool LedDeviceFile::init(const QJsonObject &deviceConfig)
{
	bool initOK = LedDevice::init(deviceConfig);

	_fileName = deviceConfig["output"].toString("/dev/null");

#if _WIN32
	if (_fileName == "/dev/null" )
	{
		_fileName = "NULL";
	}
#endif

	_printTimeStamp = deviceConfig["printTimeStamp"].toBool(false);

	initFile(_fileName);

	return initOK;
}

void LedDeviceFile::initFile(const QString &fileName)
{
	if ( _file == nullptr )
	{
		_file = new QFile(fileName, this);
	}
}

int LedDeviceFile::open()
{
	int retval = -1;
	_isDeviceReady = false;

	if ( ! _file->isOpen() )
	{
		Debug(_log, "QIODevice::WriteOnly, %s", QSTRING_CSTR(_fileName));
		_isDeviceReady = true;
		retval = 0;
	}
	return retval;
}

int LedDeviceFile::close()
{
	int retval = 0;

	_isDeviceReady = false;
	if ( _file != nullptr)
	{
		// Test, if device requires closing
		if ( _file->isOpen() )
		{
			// close device physically
			Debug(_log,"File: %s", QSTRING_CSTR(_fileName) );
			_file->close();
		}
	}
	return retval;
}

int LedDeviceFile::write(const std::vector<ColorRgb> & ledValues)
{
	_file->open(QIODevice::WriteOnly);
	for (const ColorRgb& color : ledValues)
    {
        _file->write(reinterpret_cast<const char*>(&color), 3);
    }
	_file->close();

	return 0;
}
