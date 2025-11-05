//HERE'S THE DFS FROM THE DSA WEEK 10 CODE HOMEWORK (28.03.2025)
//I used this idea to represent the DFS in ITAI Assignment 1 Backtracking
import java.util.ArrayList;
import java.util.List;
public class Main {
    public static void main(String[] args) {
        //Scanning from the input
        java.util.Scanner scanner = new java.util.Scanner(System.in);
        int N = scanner.nextInt();
        scanner.nextLine();

        String[] words = new String[N];
        for (int i = 0; i < N; i++) {
            words[i] = scanner.nextLine();
        }

        Graph<Character, Integer> graph = new Graph<>();
        HashMap<Character, Graph<Character, Integer>.Vertex> vertexMap = new HashMap<>(26);

        // Create vertices for all unique characters
        for (String word : words) {
            for (char c : word.toCharArray()) {
                if (vertexMap.get(c) == null) {
                    Graph<Character, Integer>.Vertex v = graph.addVertex(c);
                    vertexMap.put(c, v);
                }
            }
        }
        //The one of the most complicated parts:
        // Build edges based on word comparisons
        //Explanation: we go through each word and each letter in it to find the first differing letter between them (continue...)
        for (int i = 0; i < N - 1; i++) {
            String word1 = words[i];
            String word2 = words[i + 1];
            int minLength = Math.min(word1.length(), word2.length());
            for (int j = 0; j < minLength; j++) {
                char c1 = word1.charAt(j);
                char c2 = word2.charAt(j);
                if (c1 != c2) {
                    Graph<Character, Integer>.Vertex v1 = vertexMap.get(c1);
                    Graph<Character, Integer>.Vertex v2 = vertexMap.get(c2);
                    //(...continue) When it finds a mismatch, it creates a directed edge from the letter in the first word to the letter in the second word. 
                    graph.addEdge(v1, v2, 1);
                    break;
                }
            }
        }
        graph.MariaKarpova_dfs();
        if (graph.Cycle()) {
            System.out.println("Doh");
        } else {
            System.out.println(graph.getOutput());
        }
    }
}
//It's my code for the Coding exercises week 3 (i didn't find any info whether we can use built-in map/hashmap so it's easier to implement by myself (or just ctrl_c + ctrl_v from the Code Ex))
interface Map<K, V> {
    int size();
    boolean isEmpty();
    V get(K key);
    void put(K key, V value);
    void remove(K key);
}

class Entry<K, V> {
    K key;
    V value;

    public void setKey(K key) {
        this.key = key;
    }

    public void setValue(V value) {
        this.value = value;
    }

    public K getKey() {
        return key;
    }

    public V getValue() {
        return value;
    }

    public Entry(K key, V value) {
        this.key = key;
        this.value = value;
    }
}

class HashMap<K, V> implements Map<K, V> {
    int mapSize;
    int capacity;
    List<Entry<K, V>>[] hashTable;

    public HashMap(int capacity) {
        this.mapSize = 0;
        this.capacity = capacity;
        this.hashTable = new List[capacity];
        for (int i = 0; i < capacity; i++) {
            this.hashTable[i] = new ArrayList<>();
        }
    }

    public int size() {
        return this.mapSize;
    }

    public boolean isEmpty() {
        return this.mapSize == 0;
    }

    private Entry<K, V> getEntry(K key) {
        int hash = Math.abs(key.hashCode()) % capacity;
        for (Entry<K, V> e : hashTable[hash]) {
            if (e.key.equals(key)) {
                return e;
            }
        }
        return null;
    }

    public V get(K key) {
        Entry<K, V> e = this.getEntry(key);
        if (e != null) {
            return e.getValue();
        }
        return null;
    }

    public void put(K key, V value) {
        int hash = Math.abs(key.hashCode()) % capacity;
        Entry<K, V> e = this.getEntry(key);
        if (e != null) {
            e.setValue(value);
        } else {
            this.hashTable[hash].add(new Entry<>(key, value));
            this.mapSize++;
        }
    }

    public void remove(K key) {
        int hash = Math.abs(key.hashCode()) % capacity;
        Entry<K, V> e = this.getEntry(key);
        if (e != null) {
            this.hashTable[hash].remove(e);
            this.mapSize--;
        }
    }
}
//It's again my code for the Coding exercises week 2 (i again didn't find any info whether we can use built-in stack so it's easier to implement by myself (or just ctrl_c + ctrl_v from the Code Ex))
interface StackADT<T> {
    void push(T item);
    T pop();
    T peek();
    int size();
    boolean isEmpty();
}

class Node<T> {
    T value;
    Node<T> next;

    public Node(T value, Node<T> next) {
        this.value = value;
        this.next = next;
    }
}

class LinkedStack<T> implements StackADT<T> {
    private Node<T> head;
    private int size;

    public LinkedStack() {
        this.head = null;
        this.size = 0;
    }

    @Override
    public void push(T item) {
        head = new Node<>(item, head);
        size++;
    }

    @Override
    public T pop() {
        if (isEmpty()) {
            throw new RuntimeException("Stack is empty");
        }
        T value = head.value;
        head = head.next;
        size--;
        return value;
    }

    @Override
    public T peek() {
        if (isEmpty()) {
            throw new RuntimeException("Stack is empty");
        }
        return head.value;
    }

    @Override
    public int size() {
        return size;
    }

    @Override
    public boolean isEmpty() {
        return size == 0;
    }
}
//I took the implementation from the lab
class Graph<V, E> {
    class Vertex {
        V value;
        List<Edge> adjacent;

        public Vertex(V value) {
            this.value = value;
            this.adjacent = new ArrayList<Edge>();
        }
    }

    class Edge {
        Vertex from;
        Vertex to;
        E label;

        public Edge(Vertex from, Vertex to, E label) {
            this.from = from;
            this.to = to;
            this.label = label;
        }
    }

    List<Vertex> vertices;
    List<Edge> edges;
    private String color[]; //The beginning of the topological sort: i used the idea with colors from the Tutorial 10 slide 9
    private final LinkedStack<Vertex> stack;
    private boolean Cycle; //it's for "Doh"

    public Graph() {
        this.vertices = new ArrayList<Vertex>();
        this.edges = new ArrayList<Edge>();
        this.stack = new LinkedStack<>();
        this.Cycle = false;
    }

    Vertex addVertex(V value) {
        Vertex v = new Vertex(value);
        this.vertices.add(v);
        return v;
    }

    Edge addEdge(Vertex from, Vertex to, E label) {
        Edge edge = new Edge(from, to, label);
        this.edges.add(edge);
        from.adjacent.add(edge);
        return edge;
    }

    public void MariaKarpova_dfs() {
        color = new String[vertices.size()];
        for (int i = 0; i < vertices.size(); i++) {
            color[i] = "WHITE"; //So, the idea is: WHITE means that the vertex hadn't been visited before, at the beginning all our vertices are not visited
        }

        for (Vertex v : vertices) {
            if (color[vertices.indexOf(v)].equals("WHITE")) { //indexOf is a very nice helper for me, i found info from this site: https://www.geeksforgeeks.org/java-string-indexof/
                MariaKarpova_topsort(v);
            }
        }
    }
    //GRAY means that the current vertex is visiting so we work with this vertex exactly (so it's marked FRAY)
    //Also, this part is the implementation of slide 9 from the Tutorial 10
    private void MariaKarpova_topsort(Vertex u) {
        color[vertices.indexOf(u)] = "GRAY";

        for (Edge edge : u.adjacent) {
            Vertex v = edge.to;
            int index = vertices.indexOf(v);
            if (color[index].equals("GRAY")) {
                Cycle = true;
            }
            if (color[index].equals("WHITE")) {
                MariaKarpova_topsort(v);
            }
        }
        //BLACK means that the vertex is already visited, all our opeartions on vertex at the end should be marked BLACK to navigate
        color[vertices.indexOf(u)] = "BLACK";
        stack.push(u);
    }
    //"Doh" stuff
    boolean Cycle() {
        return Cycle;
    }
    //In DFS we have our stack, but we need to get the elements out
    String getOutput() {
        String result = "";
        while (!stack.isEmpty()) {
            result += stack.pop().value;
        }
        return result;
    }

    Vertex getVertex(V value) {
        for (Vertex v : vertices) {
            if (v.value.equals(value)) {
                return v;
            }
        }
        return null;
    }
    boolean adjacent(Vertex u, Vertex v) {
        for (Edge edge : u.adjacent) {
            if (edge.from.equals(v) || edge.to.equals(v)) {
                return true;
            }
        }
        return false;
    }

    void removeEdge(Edge edge) {
        edge.from.adjacent.remove(edge);
        edge.to.adjacent.remove(edge);
        this.edges.remove(edge);
    }

    void removeVertex(Vertex v) {
        for (Edge edge : v.adjacent) {
            if (!edge.from.equals(v)) edge.from.adjacent.remove(edge);
            if (!edge.to.equals(v)) edge.to.adjacent.remove(edge);
            this.edges.remove(edge);
        }
        this.vertices.remove(v);
    }
}

