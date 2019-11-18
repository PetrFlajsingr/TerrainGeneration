
template<typename DataContainer, bool ReadOnly, typename Key>
Config<DataContainer, ReadOnly, Key>::Config(std::string_view path) : path(std::string(path)) {
    reload();
}

template<typename DataContainer, bool ReadOnly, typename Key>
template<typename T, typename ...Keys>
std::optional<T> Config<DataContainer, ReadOnly, Key>::get(const Keys &...keys) {
    return container_traits::template find<T>(data, keys...);
}

template<typename DataContainer, bool ReadOnly, typename Key>
template<typename T>
T Config<DataContainer, ReadOnly, Key>::getDefault(const Key &key, const T &defaultValue) {
    if (auto found = get<T>(key); found.has_value()) {
        return found.value();
    }
    return defaultValue;
}

template<typename DataContainer, bool ReadOnly, typename Key>
template<typename T, typename>
Config<DataContainer, ReadOnly, Key> &Config<DataContainer, ReadOnly, Key>::set(const Key &key, const T &value) {
    container_traits::set(data, key, value);
    return *this;
}

template<typename DataContainer, bool ReadOnly, typename Key>
template<typename>
void Config<DataContainer, ReadOnly, Key>::save() {
    ConfigSaver<DataContainer> saver;
    saver.save(data, path);
}

template<typename DataContainer, bool ReadOnly, typename Key>
void Config<DataContainer, ReadOnly, Key>::reload() {
    ConfigLoader<DataContainer> loader;
    data = loader.load(path);
}