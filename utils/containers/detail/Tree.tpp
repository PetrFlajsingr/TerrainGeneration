
#include <containers/Tree.h>

template <typename T, unsigned int ChildCount> Tree<T, ChildCount>::Tree(value_type rootValue)
    : root(std::make_unique<Root>(rootValue)) {}
template <typename T, unsigned int ChildCount>
Tree<T, ChildCount> Tree<T, ChildCount>::BuildTree(std::size_t depth, const_reference_type initValue) {
  Tree result{initValue};
  initChildren(result.root.get(), initValue, depth - 1);
  return result;
}
template <typename T, unsigned int ChildCount> template <typename F> void Tree<T, ChildCount>::traverseDepthFirst(F &&callable) {
  root->traverseDepthFirst(callable);
}
template <typename T, unsigned int ChildCount> template <typename F>
void Tree<T, ChildCount>::traverseBreadthFirst(F &&callable) {
  root->traverseBreadthFirst(callable);
}
template <typename T, unsigned int ChildCount> template <typename F>
void Tree<T, ChildCount>::traverseDepthFirstIf(F &&callable) {
  root->traverseDepthFirstIf(callable);
}
template <typename T, unsigned int ChildCount> template <typename F, unsigned int C, typename>
void Tree<T, ChildCount>::preorder(F &&callable) {
  detail::preorderImpl(root.get(), callable);
}
template <typename T, unsigned int ChildCount> template <typename F, unsigned int C, typename>
void Tree<T, ChildCount>::inorder(F &&callable) {
  detail::inorderImpl(root.get(), callable);
}
template <typename T, unsigned int ChildCount> template <typename F, unsigned int C, typename>
void Tree<T, ChildCount>::postorder(F &&callable) {
  detail::postorderImpl(root.get(), callable);
}
template <typename T, unsigned int ChildCount>
void Tree<T, ChildCount>::initChildren(Node<T, ChildCount> *node, const_reference_type initValue, std::size_t depth) {
  NodeType nodeType = NodeType::Node;
  if (depth == 0) {
    nodeType = NodeType::Leaf;
  }
  node->setChildrenValues(initValue, nodeType);
  if (nodeType == NodeType::Leaf) {
    return;
  }
  for (auto &child : node->getChildren()) {
    initChildren(&child->asNode(), initValue, depth - 1);
  }
}

template <typename T, unsigned int ChildCount>
void Tree<T, ChildCount>::setParent(Node<T, ChildCount> *parent, Leaf<T, ChildCount> *node) {
  node->parent = parent;
  if (node->getType() == NodeType::Leaf) {
    return;
  }
  for (auto &child : node->asNode().getChildren()) {
    setParent(&node->asNode(), child.get());
  }
};

template <typename T, unsigned int ChildCount> Tree<T, ChildCount>::Tree(const Tree &other) {
  root = other.root == nullptr ? nullptr : std::make_unique<Root>(other.root->asNode());
  if (root != nullptr) {
    setParent(nullptr, root.get());
  }

  /* std::vector<Leaf<T, ChildCount>*> parentPtrs;
   std::size_t levelSize = 1;
   std::size_t previousLevelSize = 0;
   std::size_t previousLevelsSize = 0;
   std::size_t currentIndex = 0;
   root->traverseBreadthFirstNode([this, &parentPtrs, &levelSize, &previousLevelsSize, &previousLevelSize, &currentIndex] (Leaf<T,
   ChildCount> &node) { parentPtrs.emplace_back(&node); if (currentIndex == 0) { node.parent = nullptr; } else { const auto
   indexInLevel = currentIndex - previousLevelsSize; const auto parentIndexInLevel = indexInLevel / ChildCount; const auto
   previousLevelStartIndex = previousLevelsSize - previousLevelSize; const auto parentIndex = previousLevelStartIndex +
   parentIndexInLevel; node.parent = reinterpret_cast<Node<T, ChildCount>*>(parentPtrs[parentIndex]);
     }
     if (currentIndex == previousLevelsSize + levelSize) {
       previousLevelSize = levelSize;
       previousLevelsSize += levelSize;
       levelSize *= ChildCount;
     }
     ++currentIndex;
   });*/
}
template <typename T, unsigned int ChildCount> Tree<T, ChildCount> &Tree<T, ChildCount>::operator=(const Tree &other) {
  if (&other == this) {
    return *this;
  }
  root = other.root == nullptr ? nullptr : std::make_unique<Root>(other.root->asNode());
  if (root != nullptr) {
    setParent(nullptr, root.get());
  }
  return *this;
}
template <typename T, unsigned int ChildCount> template <typename F>
void Tree<T, ChildCount>::traverseDepthFirstIfNode(F &&callable) {
  root->traverseDepthFirstIfNode(callable);
}
