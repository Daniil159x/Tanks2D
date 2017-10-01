#ifndef MAPFIELD_HPP
#define MAPFIELD_HPP

#include <QImage>
#include <QJsonObject>

#include <memory>
#include <vector>

#include "sprite.hpp"

class MapField
{
public:
    MapField(const QString &fileName);

    QSize fieldSize() const;

    std::vector<std::unique_ptr<Sprite> > getFiledSprites();

private:
    // TODO: добавить имя карт

    void parseJson();

    QString m_jsonFile;
    QString m_fileField;
    QString m_prefixFile;

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
};

#endif // MAPFIELD_HPP
