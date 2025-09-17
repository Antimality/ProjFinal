from math import sqrt


def euclidean_distance(tup1: tuple, tup2: tuple) -> float:
    # Check dimentional consistancy
    if not len(tup1) == len(tup2):
        print("An Error Has Occurred")
        exit(1)
    distance = 0
    for i in range(len(tup1)):
        distance += (tup1[i] - tup2[i]) ** 2
    return sqrt(distance)


def generate_clusters(
    points: list, k: int, max_iter: int, epsilon: float
) -> list[tuple]:
    """
    defines a double list, each sub list represent a cluster
    in each sub list the first value is the center of the cluster
    and the following values are indexes of Xi in the cluster
    """
    centroids = [points[i] for i in range(k)]
    iter = 0
    flag = True
    # flag represents if a cluster center had been updated by a value more distanced the epsilon
    while (iter < max_iter) and flag:
        flag = False
        clusters_indexes = [[] for i in range(k)]
        for i in range(len(points)):
            min_index = -1
            min_value = float("inf")
            for j in range(k):
                # tries to add Xi to a cluster and search for a new minimum for it
                dist = euclidean_distance(points[i], centroids[j])
                if dist < min_value:
                    min_index = j
                    min_value = dist
            clusters_indexes[min_index].append(i)
        for j in range(k):
            # Ignore empty clusters
            if len(clusters_indexes[j]) == 0:
                continue
            new_center = average_point(points, clusters_indexes[j])
            if euclidean_distance(new_center, centroids[j]) >= epsilon:
                flag = True
            centroids[j] = new_center
        iter += 1
    return centroids


def average_point(lst: list, sub_lst: list) -> tuple[float]:
    dim = len(lst[0])
    n = len(sub_lst)

    # Initialize a list of zeros for each dimension
    totals = [0] * dim

    # Sum each dimension
    for j in range(0, len(sub_lst)):
        for i in range(dim):
            totals[i] += lst[sub_lst[j]][i]

    # Compute the average for each dimension
    return tuple(tot / n for tot in totals)


def read_points(file_name):
    # Read points from file
    with open(file_name, "r") as f:
        points = []
        while True:
            line = f.readline()
            if not line:
                break
            points.append(tuple(float(val) for val in line.split(",")))
    return points


def run_kmeans(k, file_name, max_iter, epsilon):
    points = read_points(file_name)
    return generate_clusters(points, k, max_iter, epsilon)
