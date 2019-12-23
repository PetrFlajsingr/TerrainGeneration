
#include <containers/Tree.h>

template <typename T, unsigned int ChildCount, typename F>
void detail::traverseDepthFirstImpl(Leaf<T, ChildCount> *node, F &callable) {
  if (node == nullptr) {
    return;
  }
  callable(node->getValue());
  if (node->getType() == NodeType::Leaf) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, ChildCount> *>(node);
  for (auto &child : notLeafNode->getChildren()) {
    traverseDepthFirstImpl(child.get(), callable);
  }
}
template <typename T, unsigned int ChildCount, typename F>
void detail::traverseDepthFirstIfImpl(Leaf<T, ChildCount> *node, F &callable) {
  if (node == nullptr) {
    return;
  }
  const bool shouldContinue = callable(node->getValue());
  if (node->getType() == NodeType::Leaf || !shouldContinue) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, ChildCount> *>(node);
  for (auto &child : notLeafNode->getChildren()) {
    traverseDepthFirstIfImpl(child.get(), callable);
  }
}

template <typename T, unsigned int ChildCount, typename F>
void detail::traverseDepthFirstNodeImpl(Leaf<T, ChildCount> *node, F &callable) {
  if (node == nullptr) {
    return;
  }
  callable(*node);
  if (node->getType() == NodeType::Leaf) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, ChildCount> *>(node);
  for (auto &child : notLeafNode->getChildren()) {
    traverseDepthFirstNodeImpl(child.get(), callable);
  }
}

template <typename T, unsigned int ChildCount, typename F>
void detail::traverseDepthFirstIfNodeImpl(Leaf<T, ChildCount> *node, F &callable) {
  if (node == nullptr) {
    return;
  }
  const bool shouldContinue = callable(*node);
  if (node->getType() == NodeType::Leaf || !shouldContinue) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, ChildCount> *>(node);
  for (auto &child : notLeafNode->getChildren()) {
    traverseDepthFirstIfNodeImpl(child.get(), callable);
  }
}
template <typename T, unsigned int ChildCount, typename F>
void detail::traverseBreadthFirstImpl(Leaf<T, ChildCount> *node, F &callable) {
  if (node == nullptr) {
    return;
  }
  if (node->getType() == NodeType::Leaf) {
    return;
  }

  std::queue<Node<T, ChildCount> *> queue;
  queue.push(reinterpret_cast<Node<T, ChildCount> *>(node));

  while (!queue.empty()) {
    auto currentNode = queue.front();
    queue.pop();
    if (currentNode == nullptr) {
      continue;
    }
    callable(currentNode->getValue());
    if (currentNode->getType() == NodeType::Leaf) {
      continue;
    }
    for (auto &child : currentNode->getChildren()) {
      queue.push(reinterpret_cast<Node<T, ChildCount> *>(child.get()));
    }
  }
}
template <typename T, unsigned int ChildCount, typename F>
void detail::traverseBreadthFirstNodeImpl(Leaf<T, ChildCount> *node, F &callable) {
  if (node == nullptr) {
    return;
  }
  if (node->getType() == NodeType::Leaf) {
    return;
  }

  std::queue<Node<T, ChildCount> *> queue;
  queue.push(reinterpret_cast<Node<T, ChildCount> *>(node));

  while (!queue.empty()) {
    auto currentNode = queue.front();
    queue.pop();
    if (currentNode == nullptr) {
      continue;
    }
    callable(*currentNode);
    if (currentNode->getType() == NodeType::Leaf) {
      continue;
    }
    for (auto &child : currentNode->getChildren()) {
      queue.push(reinterpret_cast<Node<T, ChildCount> *>(child.get()));
    }
  }
}
template <typename T, typename F> void detail::preorderImpl(Leaf<T, 2> *node, F &&callable) {
  if (node == nullptr) {
    return;
  }
  callable(node->getValue());
  if (node->getType() == NodeType::Leaf) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, 2> *>(node);
  preorderImpl(&notLeafNode->leftChild(), callable);
  preorderImpl(&notLeafNode->rightChild(), callable);
}
template <typename T, typename F> void detail::inorderImpl(Leaf<T, 2> *node, F &&callable) {
  if (node == nullptr) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, 2> *>(node);
  if (node->getType() != NodeType::Leaf) {
    preorderImpl(&notLeafNode->leftChild(), callable);
  }
  callable(node->getValue());
  if (node->getType() != NodeType::Leaf) {
    preorderImpl(&notLeafNode->rightChild(), callable);
  }
}
template <typename T, typename F> void detail::postorderImpl(Leaf<T, 2> *node, F &&callable) {
  if (node == nullptr) {
    return;
  }
  auto notLeafNode = reinterpret_cast<Node<T, 2> *>(node);
  if (node->getType() != NodeType::Leaf) {
    preorderImpl(&notLeafNode->leftChild(), callable);
  }
  if (node->getType() != NodeType::Leaf) {
    preorderImpl(&notLeafNode->rightChild(), callable);
  }
  callable(node->getValue());
}
