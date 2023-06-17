#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

std::string ReadFromFile(std::string_view path) {
	std::ifstream stream(path.data());
	std::string line;
	std::string ret = "";

	while (std::getline(stream, line)) {
		ret.append(line);
		ret.append("\n");
	}

	stream.close();

	return ret;
}

struct Node {

	Node* FindLast() {
		if (child.get()) {
			return child->FindLast();
		}

		return this;
	}

	Node* FindNode(char key) {
		if (key == this->key) {
			return this;
		}

		if (child) {
			return child->FindNode(key);
		}

		return nullptr;
	}

	std::unique_ptr<Node> child{};
	Node* parent = nullptr;

	char key;
	size_t count = 0;
};

std::string GetAsString(const std::vector<bool>& value) {
	std::string output = "";
	for (const bool b : value) {
		output += b ? "1" : "0";
	}

	return output;
}

int main() {
	std::cout << "XD" << std::endl;

	const auto data = ReadFromFile("data.txt");

	std::unique_ptr<Node> root{};

	for (const char c : data) {
		if (root.get() == nullptr) {
			root = std::make_unique<Node>();

			root->count = 1;
			root->key = c;

			continue;
		}

		Node* node = root->FindNode(c);

		if (node == nullptr) {
			node = root->FindLast();

			node->child = std::make_unique<Node>();

			Node* newNode = node->child.get();
			newNode->parent = node;

			newNode->count = 1;
			newNode->key = c;
		}
		else {
			node->count++;

			if (node->parent && node->count > node->parent->count) {
				std::swap(node->count, node->parent->count);
				std::swap(node->key, node->parent->key);
			}
		}
	}

	const Node* node = root.get();
	std::unordered_map<char, std::vector<bool>> map{};
	std::vector<bool> lastBinaryValue{};
	lastBinaryValue.push_back(true);
	while (node != nullptr) {
		map[node->key] = lastBinaryValue;
		lastBinaryValue.push_back(false);
		node = node->child.get();
	}

	size_t newSize = 0;
	node = root.get();
	while (node != nullptr) {
		std::cout << "Nodo: " << node->key << " count: " << node->count << " valor binario: " << GetAsString(map[node->key]) << std::endl;
		newSize += map[node->key].size() * node->count;
		node = node->child.get();
	}

	std::cout << "Tamaño teórico = " << data.size() * 8 << " bits" << std::endl;
	std::cout << "Tamaño nuevo = " << newSize << " bits" << std::endl;

	std::cout << data << std::endl;

	return 0;
}
