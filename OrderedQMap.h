#ifndef ORDEREDQMAP_H
#define ORDEREDQMAP_H
#include <QMap>
#include <QVariant>
#include <QList>
#include <QString>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QDebug>

/**
* \brief template OrderedQMap and OrderedQMultiMap class provides an interface for initializable ordered QMap<Key, T> with  (data)stream operators << >>
*
* \code
* OrderedQMap <QString, int> map;
* map.insert("3",3);
* map.insert("2",2);
* map.insert("1",1);
*
* for(int i = 0; i < map.size(); ++i){
*   qDebug() << "map.at" << QString::Number(i) << "=" << map.at(i);
* }
*
* \endcode
*
* \code
*    //  example of using << operator to define initialized map:
*    //  *.h file:
*        class NamedYearParts
*        {
*           public:
*           static const OrderedQMap<QString, int> DayOfWeek;
*         };
*
*    // *.cpp file
*    //  const OrderedQMap<QString, int> NamedYearParts::DayOfWeek = OrderedQMap<QString,int>()
*        << QPair<QString, int>("Sunday", 1)
*        << QPair<QString, int>("Monday", 2)
*        << QPair<QString, int>("Tuesday", 3)
*        << QPair<QString, int>("Wednesday", 4)
*        << QPair<QString, int>("Thursday", 5)
*        << QPair<QString, int>("Friday", 6)
*        << QPair<QString, int>("Saturday", 7)
* * \endcode
*
* OrderedQMap provides a Fluent Interface API.
*
* \file OrderedQMap.h
*/

namespace ActionNet
{

    //ordered QMap that must have UNIQUE keys to get correct results of indexed (...at) methods
    template <class Key, class T> class OrderedQMap : public QMap<Key,T>, private QList<Key>
    {
        //ToDo
        // Do not use specified functions of QMap until they are not implemented here:
        //    erase(), insertMulti(), swap(), take(), unite()
        // Do not use write functions of QList until they are not implemented here.
    public:
        using typename QMap<Key,T>::iterator;
        using typename QMap<Key,T>::const_iterator;

        typename QMap<Key,T>::iterator begin() { return QMap<Key,T>::begin(); }

        typename QMap<Key,T>::const_iterator begin() const { return QMap<Key,T>::begin(); }

        typename QMap<Key,T>::iterator end() { return QMap<Key,T>::end(); }

        typename QMap<Key,T>::const_iterator end() const { return QMap<Key,T>::end(); }

        //overrides QList::contains method
        bool contains(const Key & key) const { return QList<Key>::contains(key); }

        bool contains(const QString & key, Qt::CaseSensitivity cs) const
        {
          //QList<QString> keys = this->keys();
          QStringList keys = this->keys();
          return keys.contains(key, cs);
        }

        //overrides QList::operator method
        const T operator[](Key & key) const { return QMap<Key,T>::value(key); }

        T & operator[](Key & key)
        {
          if (!QList<Key>::contains(key))
             QList<Key>::append(key);
          return QMap<Key,T>::operator [](key);
        }

        //overrides QMap::operator method
        const T operator[](const Key & key) const { return QMap<Key,T>::value(key); }

        T & operator[](const Key & key)
        {
          if (!QList<Key>::contains(key))
            QList<Key>::append(key);
          return QMap<Key,T>::operator [](key);
        }

        typename QMap<Key, T>::iterator insert(const Key & key, const T & value)
        {
          typename QMap<Key, T>::iterator iter = QMap<Key,T>::insert(key, value);
          if (!QList<Key>::contains(key))
             QList<Key>::append(key);
          return iter;
        }

        typename QMap<Key, T>::iterator append(const Key & key, const T & value) { return insert(key, value); }

        typename QMap<Key, T>::iterator prepend(const Key & key, const T & value)
        {
          typename QMap<Key, T>::iterator iter = QMap<Key,T>::insert(key, value);
          if (!QList<Key>::contains(key))
             QList<Key>::prepend(key);
          return iter;
        }

        Key replaceAt(int index, const T & value)
        {
          Key key = QList<Key>::at(index);
          QMap<Key,T>::insert(key, value);
          return key;
        }

        T at(int index) const
        {
           Key key = QList<Key>::at(index);
           return  QMap<Key,T>::value(key);
        }

        T value(int index) const
        {
           Key key = QList<Key>::value(index);
           return  QMap<Key,T>::value(key);
        }

        int remove(const Key &key)
        {
           int i = QList<Key>::indexOf(key);
           QList<Key>::removeAt(i);
           QMap<Key,T>::remove(key);
           return i;
        }

        Key removeAt(int i)
        {
            Key key = this->key(i);
            QList<Key>::removeAt(i);
            QMap<Key,T>::remove(key);
            return key;
        }

        Key removeLast()
        {
            Key key = QList<Key>::last();
            this->remove(key);
            return key;
        }

        QPair<Key, T> last() const
        {
           if(QList<Key>::isEmpty())
             return QPair<Key, T>();
           Key key = QList<Key>::last();
           T val = this->value(key);
           return QPair<Key, T>(key, val);
        }

        bool isEmpty() const
        { return QMap<Key,T>::isEmpty(); }

        //overrides QList value methods
        const T value(Key & key) const
        { return QMap<Key,T>::value(key, T()); }

        const T value(Key & key, const T & defaultValue) const
        { return QMap<Key,T>::value(key, defaultValue); }

        //overrides QMap value methods
        const T value(const Key & key) const
        { return QMap<Key,T>::value(key, T()); }

        const T value(const Key & key, const T & defaultValue) const
        { return QMap<Key,T>::value(key, defaultValue); }

        QMap<Key, T> toQMap() { return *this; }

        QList<T> values() const
        {
          QList<T> res;
          QList<Key> keys = this->keys();
          for (Key key : keys)
            res.append(this->value(key));
          return res;
        }

        QList<QPair<T, Key> > valueKeyList(bool addEmptyPair = false, bool putEmptyPairAsFirst = true) const
        {
          QList<QPair<T, Key> > res;
          QList<Key> keys = this->keys();
          for (Key key : keys)
            res.append(qMakePair(this->value(key), key));
          if (addEmptyPair)
          {
              if (putEmptyPairAsFirst)
                res.prepend(qMakePair(T(), Key()));
              else
                res.append(qMakePair(T(), Key()));
          }
          return res;
        }

        Key key(int index) const { return QList<Key>::value(index);}

        int size() const { return QMap<Key,T>::size(); }

        int count(const Key &key) const { return QMap<Key,T>::count(key); }

        int length() const { return QMap<Key, T>::size(); }

        QList<Key> keys() const { return *this; }

        int keyOrder(const Key &key) const
        {
            auto keysTmp = keys();
            return keysTmp.indexOf(key);
        }

        void clear()
        {
           QMap<Key,T>::clear();
           QList<Key>::clear();
        }

        OrderedQMap<Key, T>& operator()(const Key & key, const T & value)
        {
            QMap<Key,T>::insert(key, value);
            if (!QList<Key>::contains(key))
              QList<Key>::append(key);
            return *this;
        }

        OrderedQMap<Key,T> &operator<< (const QPair<Key,T> &t)
        {
          this->insert(t.first, t.second);
          return *this;
        }

        OrderedQMap<Key,T> &operator>> (const QPair<Key,T> &t)
        {
          this->prepend(t.first, t.second);
          return *this;
        }

        friend QDataStream &operator <<(QDataStream &out, const OrderedQMap<Key,T> &obj)
        {
          QList<Key> keys = obj.keys();
          QMap<Key, T> tmpMap;
          for(Key k : keys)
              tmpMap.insert(k, obj.value(k));
          out << obj.keys() << tmpMap;
          return out;
        }

        friend QDataStream &operator >>(QDataStream &in, OrderedQMap<Key,T> &obj)
        {
          QList<Key> keys;
          QMap<Key, T> tmpMap;
          in >> keys >> tmpMap;
          for(Key k : keys)
             obj.insert(k, tmpMap.value(k));
          return in;
        }

    };

    template <class Key, class T> class OrderedQMultiMap : public QMultiMap<Key,T>, private QList<Key>
    {
        //ToDo
        // Do not use specified functions of QMultiMap until they are not implemented here:
        //    erase(), swap(), take(), unite()
        // Do not use write functions of QList until they are not implemented here.

    public:
        //overrides QList::contains method
        bool contains(const Key & key) const { return QList<Key>::contains(key); }

        //introduces case (in)sensitive contains lije QStringList has.
        bool contains(const Key & key, Qt::CaseSensitivity cs) const
        {
          QString strKey = QVariant(key).toString();
          QList<Key> keys = this->keys();
          for (Key k : keys)
          {
            QString str = QVariant(k).toString();
            if (strKey.compare(str, cs) == 0)
              return true;
          }
          return false;
        }

        //overrides QList::operator method
        const T operator[](Key & key) const { return QMultiMap<Key,T>::value(key); }

        T & operator[](Key & key)
        {
          if (!QList<Key>::contains(key))
            QList<Key>::append(key);
          return QMap<Key,T>::operator [](key);
        }

        //overrides QMultiMap::operator method
        const T operator[](const Key & key) const { return QMultiMap<Key,T>::value(key); }

        T & operator[](const Key & key)
        {
          if (!QList<Key>::contains(key))
            QList<Key>::append(key);

          return QMultiMap<Key,T>::operator [](key);
        }

        typename QMap<Key, T>::iterator insert(const Key & key, const T & value)
        {
          typename QMap<Key, T>::iterator iter = QMultiMap<Key,T>::insert(key, value);
          QList<Key>::append(key);
          return iter;
        }

        typename QMap<Key, T>::iterator append(const Key & key, const T & value) { return insert(key, value); }

        typename QMap<Key, T>::iterator prepend(const Key & key, const T & value)
        {
          typename QMap<Key, T>::iterator iter = QMultiMap<Key,T>::insert(key, value);
          if (!QList<Key>::contains(key))
             QList<Key>::prepend(key);
          return iter;
        }

        typename QMap<Key, T>::iterator replace(const Key & key, const T & value)
        {
          if (!QList<Key>::contains(key))
            QList<Key>::append(key);
          typename QMap<Key, T>::iterator iter = QMultiMap<Key,T>::replace(key, value);
          return iter;
        }

        typename QMap<Key, T>::iterator find(const Key & key) const { return QMultiMap<Key,T>::find(key); }

        int remove(const Key &key)
        {
           QList<Key>::removeAll(key); //QList<Key>::removeAt(i);
           int i = QMultiMap<Key,T>::remove(key);
           return i;
        }

        T last() const
        {
           if(QList<Key>::isEmpty())
              return T();
           Key key = QList<Key>::last();
           return this->value(key);
        }

        bool isEmpty() const { return QMultiMap<Key,T>::isEmpty(); }

        //overrides QList value methods
        const T value(Key & key) const { return QMultiMap<Key,T>::value(key, T()); }

        const T value(Key & key, const T & defaultValue) const { return QMultiMap<Key,T>::value(key, defaultValue); }

        //overrides QMultiMap value methods
        const T value(const Key & key) const { return QMultiMap<Key,T>::value(key, T()); }

        const T value(const Key & key, const T & defaultValue) const { return QMultiMap<Key,T>::value(key, defaultValue); }

        Key key(int index) const { return QList<Key>::value(index); }

        int size() const { return QMultiMap<Key,T>::size(); }

        int count(const Key &key) const { return QMap<Key,T>::count(key); }

        int length() const { return QMap<Key, T>::size(); }

        QList<Key> keys() const {return *this;}

        void clear()
        {
           QMultiMap<Key,T>::clear();
           QList<Key>::clear();
        }

        OrderedQMultiMap<Key, T>& operator()(const Key & key, const T & value)
        {
            QMultiMap<Key,T>::insert(key, value);
            QList<Key>::append(key);
            return *this;
        }


        OrderedQMultiMap<Key,T> &operator<< (const QPair<Key,T> &t)
        {
          this->insert(t.first, t.second);
          return *this;
        }

        friend QDataStream &operator <<(QDataStream &out, const OrderedQMultiMap<Key,T> &obj)
        {
          QList<Key> keys = obj.keys();
          QMultiMap<Key, T> tmpMap;
          for(Key k : keys)
              tmpMap.insert(k, obj.value(k));
          out << obj.keys() << tmpMap;
          return out;
        }

        friend QDataStream &operator >>(QDataStream &in, OrderedQMultiMap<Key,T> &obj)
        {
          QList<Key> keys;
          QMultiMap<Key, T> tmpMap;
          in >> keys >> tmpMap;
          for(Key k : keys)
                obj.insert(k, tmpMap.value(k));
          return in;
        }

        QMultiMap<Key,T> toQMap() { return *this; }

    };

}

typedef ActionNet::OrderedQMap<QString, QVariant> QVariantOrderedQMap;
Q_DECLARE_METATYPE(QVariantOrderedQMap)

#endif // ORDEREDQMAP_H
