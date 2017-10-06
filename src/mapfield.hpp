#ifndef MAPFIELD_HPP
#define MAPFIELD_HPP

#include <QImage>
#include <QJsonObject>

#include <memory>
#include <vector>

#include "sprite.hpp"
#include "playersprite.hpp"

class MapField
{
public:
    MapField(const QString &fileName);

    QSize fieldSize() const;

    std::vector<std::unique_ptr<Sprite> > getFiledSprites() /*const*/ ;

    QString getNameMap() const;

    std::tuple<std::unique_ptr<PlayerSprite>, std::unique_ptr<PlayerSprite>> getPlayers() const;

    QSize getFieldSize() const;

    const QVector<QImage> &getBulletImages() const;

private:
    void parseDataField();

    QString m_jsonFile;
    QString m_fileField;
    QString m_prefixFile;

    QString m_nameMap;

    QVector<QString> m_field;

    QSize  m_spriteSize;
    QSize  m_fieldSize;

    QJsonObject m_root;

    struct dataSprites {
        QVector<QImage> m_vecImgs;
        QSize           m_size;
        typeItems       m_type;
    };

    QHash<QChar, dataSprites> m_hashSprites;

    struct dataPlayers {
        QPoint      m_pos = {-1, -1};
        dataSprites m_data;
    } m_dataPlayers[2];

    QVector<QImage> m_BulletImages;
};

#endif // MAPFIELD_HPP
