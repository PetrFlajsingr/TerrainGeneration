//
// Created by petr on 12/21/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#ifndef UTILITIES_TREE_H
#define UTILITIES_TREE_H

#include <memory>
#include <queue>
#include <utility>

enum class NodeType { Leaf, Node };

template <typename T, unsigned int ChildCount> class Leaf;
template <typename T, unsigned int ChildCount> class Node;
template <typename T, unsigned int ChildCount> class Tree;
namespace detail {
template <unsigned int Count> static constexpr bool is_binary_tree = Count == 2;
template <unsigned int Count> using enabled_for_binary = std::enable_if_t<is_binary_tree<Count>>;
template <typename T, unsigned int ChildCount, typename F> void traverseDepthFirstImpl(Leaf<T, ChildCount> *node, F &callable);
template <typename T, unsigned int ChildCount, typename F> void traverseDepthFirstIfImpl(Leaf<T, ChildCount> *node, F &callable);
template <typename T, unsigned int ChildCount, typename F> void traverseDepthFirstNodeImpl(Leaf<T, ChildCount> *node, F &callable);
template <typename T, unsigned int ChildCount, typename F> void traverseDepthFirstIfNodeImpl(Leaf<T, ChildCount> *node, F &callable);
template <typename T, unsigned int ChildCount, typename F> void traverseBreadthFirstImpl(Leaf<T, ChildCount> *node, F &callable);
template <typename T, unsigned int ChildCount, typename F> void traverseBreadthFirstNodeImpl(Leaf<T, ChildCount> *node, F &callable);
template <typename T, typename F> void preorderImpl(Leaf<T, 2> *node, F &&callable);
template <typename T, typename F> void inorderImpl(Leaf<T, 2> *node, F &&callable);
template <typename T, typename F> void postorderImpl(Leaf<T, 2> *node, F &&callable);
} // namespace detail

template <typename T, unsigned int ChildCount> class Leaf {
  friend class Node<T, ChildCount>;
  friend class Tree<T, ChildCount>;
public:
  using value_type = T;
  using pointer_type = T *;
  using const_pointer_type = const T *;
  using reference_type = T &;
  using const_reference_type = const T &;
  using size_type = std::size_t;

  Leaf() = default;
  explicit Leaf(value_type value);
  Leaf(const Leaf &other);
  Leaf &operator=(const Leaf &other);
  Leaf(Leaf &&other) noexcept;
  Leaf &operator=(Leaf &&other) noexcept;
  /**
   * @return value stored in node
   */
  reference_type operator*() { return value; }
  /**
   * @return value stored in node
   */
  pointer_type operator->() { return &value; }
  /**
   * @return value stored in node
   */
  reference_type getValue() { return value; }
  /**
   * @return value stored in node
   */
  const_reference_type getValue() const { return value; }
  void setValue(value_type value) { Leaf::value = value; }
  /**
   * For safe casting without dynamic cast.
   */
  [[nodiscard]] virtual NodeType getType() const { return NodeType::Leaf; }

  /**
   * @see Tree::traverseDepthFirst
   */
  template <typename F> void traverseDepthFirst(F &&callable);
  /**
   * @see Tree::traverseDepthFirstIf
   */
  template <typename F> void traverseDepthFirstIf(F &&callable);
  /**
   * @see Tree::traverseDepthFirstIf
   */
  template <typename F> void traverseDepthFirstIfNode(F &&callable);
  /**
   * @see Tree::traverseDepthFirstIf
   */
  template <typename F> void traverseDepthFirstNode(F &&callable);
  /**
   * @see Tree::traverseBreadthFirst
   */
  template <typename F> void traverseBreadthFirst(F &&callable);
  /**
   * @see Tree::traverseBreadthFirst
   */
  template <typename F> void traverseBreadthFirstNode(F &&callable);

  /**
   * @see Tree::preorder
   */
  template <typename F, unsigned int C = ChildCount, typename = detail::enabled_for_binary<C>> void preorder(F &&callable);
  /**
   * @see Tree::inorder
   */
  template <typename F, unsigned int C = ChildCount, typename = detail::enabled_for_binary<C>> void inorder(F &&callable);
  /**
   * @see Tree::postorder
   */
  template <typename F, unsigned int C = ChildCount, typename = detail::enabled_for_binary<C>> void postorder(F &&callable);
  /**
   * Undefined behavior if the node has no parent.
   */
  [[nodiscard]]Node<T, ChildCount> &getParent();
  [[nodiscard]] bool isRoot() const;

  Node<T, ChildCount> &asNode();

  virtual ~Leaf() = default;

protected:
  Node<T, ChildCount> *parent = nullptr;

private:
  value_type value;
};

template <typename T, unsigned int ChildCount> class Node : public Leaf<T, ChildCount> {
  using Base = Leaf<T, ChildCount>;
  using Child = Leaf<T, ChildCount>;
  using ChildPtr = std::unique_ptr<Leaf<T, ChildCount>>;
  using Children = std::array<ChildPtr, ChildCount>;

  template <unsigned int Count> static constexpr bool is_binary_tree = Count == 2;
  template <unsigned int Count> using enabled_for_binary = std::enable_if_t<is_binary_tree<Count>>;

public:
  using value_type = typename Base::value_type;
  using pointer_type = typename Base::pointer_type;
  using const_pointer_type = typename Base::const_pointer_type;
  using reference_type = typename Base::reference_type;
  using const_reference_type = typename Base::const_reference_type;

  using Base::operator*;
  using Base::operator->;
  Node();
  explicit Node(value_type value);
  Node(const Node &other);
  Node &operator=(const Node &other);
  Node(Node &&other) noexcept;
  Node &operator=(Node &&other) noexcept;

  /**
   * @return false if child at index is nullptr
   */
  bool hasChildAtIndex(std::size_t index);
  Child &setChildAtIndex(std::size_t index, NodeType nodeType);
  void setChildrenValues(const_reference_type value, NodeType nodeType);
  Child &childAtIndex(std::size_t index);
  Children &getChildren() { return children; }

  [[nodiscard]] NodeType getType() const override { return NodeType::Node; }

  template <unsigned int C = ChildCount, typename = enabled_for_binary<C>> Child &leftChild();
  template <unsigned int C = ChildCount, typename = enabled_for_binary<C>> Child &rightChild();

private:
  Children children;
};
template <typename T, unsigned int ChildCount> class Tree {
public:
  using Root = Node<T, ChildCount>;
  using value_type = typename Root::value_type;
  using pointer_type = typename Root::pointer_type;
  using const_pointer_type = typename Root::const_pointer_type;
  using reference_type = typename Root::reference_type;
  using const_reference_type = typename Root::const_reference_type;

  Tree() = default;
  explicit Tree(value_type rootValue);
  Tree(const Tree &other);
  Tree &operator=(const Tree &other);
  /**
   * Create a tree with given depth. Each node/leaf is initialised with the give value.
   * @param depth depth of tree (root only is 1)
   * @param initValue value to copy to each node/leaf
   */
  static Tree BuildTree(std::size_t depth, const_reference_type initValue);

  Root &getRoot() { return *root; }

  /**
   * Call F for a value of each node in depth first order.
   */
  template <typename F> void traverseDepthFirst(F &&callable);
  /**
   * Call F for each node in depth first order.
   */
  template <typename F> void traverseDepthFirstIfNode(F &&callable);
  /**
   * Call F for a value of each node in breadth first order.
   */
  template <typename F> void traverseBreadthFirst(F &&callable);
  /**
   * Call F for a value of each node in depth first order.
   * If F returns false then this branch of tree will no longer be
   * iterated through.
   */
  template <typename F> void traverseDepthFirstIf(F &&callable);

  template <typename F, unsigned int C = ChildCount, typename = detail::enabled_for_binary<C>> void preorder(F &&callable);
  template <typename F, unsigned int C = ChildCount, typename = detail::enabled_for_binary<C>> void inorder(F &&callable);
  template <typename F, unsigned int C = ChildCount, typename = detail::enabled_for_binary<C>> void postorder(F &&callable);

private:
  std::unique_ptr<Root> root;

  static void initChildren(Node<T, ChildCount> *node, const_reference_type initValue, std::size_t depth);
  void setParent(Node<T, ChildCount> *parent, Leaf<T, ChildCount> *node);
};

#include "detail/Tree.tpp"
#include "detail/Tree_detail.tpp"
#include "detail/Tree_node.tpp"
#endif // UTILITIES_TREE_H

#pragma clang diagnostic pop