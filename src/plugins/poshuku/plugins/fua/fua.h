#ifndef FUA_H
#define FUA_H
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QMap>
#include <interfaces/iinfo.h>
#include <interfaces/ihavesettings.h>
#include <interfaces/iplugin2.h>
#include <interfaces/pluginbase.h>

class QStandardItemModel;
class Settings;
namespace LeechCraft
{
	namespace Util
	{
		class XmlSettingsDialog;
	};
};

class Poshuku_Fua : public QObject
				  , public IInfo
				  , public IPlugin2
				  , public IHaveSettings
				  , public LeechCraft::Poshuku::PluginBase
{
	Q_OBJECT
	Q_INTERFACES (IInfo IPlugin2 IHaveSettings LeechCraft::Poshuku::PluginBase)

	boost::shared_ptr<QStandardItemModel> Model_;
	boost::shared_ptr<Settings> Settings_;
	boost::shared_ptr<LeechCraft::Util::XmlSettingsDialog> XmlSettingsDialog_;
	QMap<QString, QString> Browser2ID_;
public:
	void Init ();
	void Release ();
	QString GetName () const;
	QString GetInfo () const;
	QIcon GetIcon () const;
	QStringList Provides () const;
	QStringList Needs () const;
	QStringList Uses () const;
	void SetProvider (QObject*, const QString&);

	QByteArray GetPluginClass () const;

	boost::shared_ptr<LeechCraft::Util::XmlSettingsDialog> GetSettingsDialog () const;

	void Init (LeechCraft::Poshuku::IProxyObject*);
	QString OnUserAgentForUrl (const QWebPage*, const QUrl&);

	void Save () const;
	const QMap<QString, QString>& GetBrowser2ID () const;
};

#endif
