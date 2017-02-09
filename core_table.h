#ifndef CORE_TABLE_H
#define CORE_TABLE_H

template <typename K, typename V>
class core_table {
	using value = std::pair<K, V>;
	using chain = std::list<value>;

	using iterator = typename chain::iterator;
	using const_iterator = typename chain::const_iterator;
	using reverse_iterator = typename chain::reverse_iterator;
	using const_reverse_iterator = typename chain::const_reverse_iterator;

	struct compare {
		using is_transparent = void;

		bool operator ()(const_iterator x, const_iterator y) const {
			return x->first < y->first;
		}

		bool operator ()(const_iterator x, K const & k) const {
			return x->first < k;
		}

		bool operator ()(K const & k, const_iterator y) const {
			return k < y->first;
		}
	};

	chain data;
	std::set<iterator, compare> gate;

public:
	core_table() = default;

	core_table(core_table && ct) = default;

	core_table(core_table const & ct) {
		for (auto const & e : ct) {
			push(e.first, e.second);
		}
	}

	core_table(std::initializer_list<value> il) {
		for (auto const & e : il) {
			push(e.first, e.second);
		}
	}

	template <typename Key, typename Value>
	core_table & insert(const_iterator cit, Key && k, Value && v) {
		auto const it = gate.find(k);
		if (it != gate.end()) {
			(*it)->second = std::forward<Value>(v);
			return *this;
		}
		gate.emplace(data.emplace(cit, std::forward<Key>(k), std::forward<Value>(v)));
		return *this;
	}

	template <typename Key, typename Value>
	core_table & unshift(Key && k, Value && v) {
		return insert(begin(), std::forward<Key>(k), std::forward<Value>(v));
	}

	core_table & prepend(core_table && ct) {
		for (auto & e : ct) {
			unshift(std::move(e.first), std::move(e.second));
		}
		return *this;
	}

	core_table & prepend(core_table const & ct) {
		for (auto const & e : ct) {
			unshift(e.first, e.second);
		}
		return *this;
	}

	template <typename Key, typename Value>
	core_table & push(Key && k, Value && v) {
		return insert(end(), std::forward<Key>(k), std::forward<Value>(v));
	}

	core_table & append(core_table && ct) {
		for (auto & e : ct) {
			push(std::move(e.first), std::move(e.second));
		}
		return *this;
	}

	core_table & append(core_table const & ct) {
		for (auto const & e : ct) {
			push(e.first, e.second);
		}
		return *this;
	}

	core_table & splice(const_iterator cit, core_table && ct) {
		for (auto & e : ct) {
			insert(cit, std::move(e.first), std::move(e.second));
		}
		return *this;
	}

	core_table & splice(const_iterator cit, core_table const & ct) {
		for (auto const & e : ct) {
			insert(cit, e.first, e.second);
		}
		return *this;
	}

	core_table remove(K const & k) {
		auto const it = gate.find(k);
		if (it != gate.end()) {
			core_table output {{(*it)->first, (*it)->second}};
			data.erase(*it);
			gate.erase(it);
			return output;
		}
		return {};
	}

	core_table shift() {
		if (is_empty()) {
			return {};
		}
		return remove(front().first);
	}

	core_table pop() {
		if (is_empty()) {
			return {};
		}
		return remove(back().first);
	}

	core_table & clear() noexcept {
		data.clear();
		gate.clear();
		return *this;
	}

	template <typename Predicate>
	core_table & sort(Predicate p) {
		data.sort(p);
		return *this;
	}

	core_table & sort_by_key() {
		data.sort([](value const & x, value const & y) {
			return x.first < y.first;
		});
		return *this;
	}

	core_table & sort_by_value() {
		data.sort([](value const & x, value const & y) {
			return x.second < y.second;
		});
		return *this;
	}

	core_table & swap(core_table & ct) {
		data.swap(ct.data);
		gate.swap(ct.gate);
		return *this;
	}

	core_table & reverse() noexcept {
		data.reverse();
		return *this;
	}

	core_table<V, K> invert() const {
		core_table<V, K> output;
		for (auto const & e : data) {
			output.push(e.second, e.first);
		}
		return output;
	}

	bool is_empty() const noexcept {
		return gate.empty();
	}

	bool contains_key(K const & k) const {
		return gate.find(k) != gate.end();
	}

	bool contains_value(V const & v) const {
		for (auto const & e : data) {
			if (e.second == v) {
				return true;
			}
		}
		return false;
	}

	size_t length() const noexcept {
		return gate.size();
	}

	iterator begin() noexcept {
		return data.begin();
	}

	iterator end() noexcept {
		return data.end();
	}

	const_iterator begin() const noexcept {
		return data.cbegin();
	}

	const_iterator end() const noexcept {
		return data.cend();
	}

	const_iterator cbegin() const noexcept {
		return data.cbegin();
	}

	const_iterator cend() const noexcept {
		return data.cend();
	}

	reverse_iterator rbegin() noexcept {
		return data.rbegin();
	}

	reverse_iterator rend() noexcept {
		return data.rend();
	}

	const_reverse_iterator rbegin() const noexcept {
		return data.crbegin();
	}

	const_reverse_iterator rend() const noexcept {
		return data.crend();
	}

	const_reverse_iterator crbegin() const noexcept {
		return data.crbegin();
	}

	const_reverse_iterator crend() const noexcept {
		return data.crend();
	}

	iterator find(K const & k) {
		auto const it = gate.find(k);
		return it != gate.end() ? std::next(*it) : end();
	}

	const_iterator find(K const & k) const {
		auto const it = gate.find(k);
		return it != gate.end() ? std::next(*it) : end();
	}

	value & front() {
		return data.front();
	}

	value const & front() const {
		return data.front();
	}

	value & back() {
		return data.back();
	}

	value const & back() const {
		return data.back();
	}

	core_table head() const {
		if (is_empty()) {
			return {};
		}
		return {{front().first, front().second}};
	}

	core_table last() const {
		if (is_empty()) {
			return {};
		}
		return {{back().first, back().second}};
	}

	core_table lead() const {
		if (is_empty()) {
			return {};
		}
		core_table output {*this};
		output.pop();
		return output;
	}

	core_table tail() const {
		if (is_empty()) {
			return {};
		}
		core_table output {*this};
		output.shift();
		return output;
	}

	core_table edge() const {
		if (length() < 3) {
			return {*this};
		}
		return head().append(last());
	}

	core_table bulk() const {
		if (length() < 3) {
			return {};
		}
		core_table output {*this};
		output.shift();
		output.pop();
		return output;
	}

	std::vector<K> keys() const {
		std::vector<K> output;
		output.reserve(length());
		for (auto const & e : data) {
			output.emplace_back(e.first);
		}
		return output;
	}

	std::vector<V> values() const {
		std::vector<V> output;
		output.reserve(length());
		for (auto const & e : data) {
			output.emplace_back(e.second);
		}
		return output;
	}

	V value_at(K const & k) const {
		auto const it = gate.find(k);
		if (it != gate.end()) {
			return (*it)->second;
		}
		return {};
	}

	template <typename ...Arguments>
	std::vector<V> values_at(Arguments const & ...a) const {
		return {value_at(a)...};
	}

	V & operator [](K const & k) {
		auto const it = gate.find(k);
		if (it != gate.end()) {
			return (*it)->second;
		}
		throw std::out_of_range("core_table, operator []: key not found!");
	}

	V const & operator [](K const & k) const {
		auto const it = gate.find(k);
		if (it != gate.end()) {
			return (*it)->second;
		}
		throw std::out_of_range("core_table, operator []: key not found!");
	}

	core_table & operator =(core_table && ct) = default;

	core_table & operator =(core_table const & ct) {
		return swap(core_table {ct});
	}

	core_table & operator =(std::initializer_list<value> il) {
		return swap(core_table {il});
	}

	explicit operator bool() const {
		return !is_empty();
	}

	bool operator ==(core_table const & ct) const {
		return data == ct.data;
	}

	bool operator !=(core_table const & ct) const {
		return !(*this == ct);
	}

	bool operator <(core_table const & ct) const {
		return data < ct.data;
	}

	bool operator >(core_table const & ct) const {
		return ct < *this;
	}

	bool operator <=(core_table const & ct) const {
		return !(*this > ct);
	}

	bool operator >=(core_table const & ct) const {
		return !(*this < ct);
	}
};

#endif
