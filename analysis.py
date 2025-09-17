import sys
import numpy as np
from sklearn.metrics import silhouette_score
from kmeans import run_kmeans
from symnmf import run_symnmf


def main():
    """Main execution function for analysis."""
    try:
        k = int(sys.argv[1])
        file_name = sys.argv[2]
        data_points = np.loadtxt(file_name, delimiter=",")

        # Generate symNMF labels
        h_final = run_symnmf(k, "symnmf", file_name)
        h_final_np = np.array(h_final)
        nmf_labels = np.argmax(h_final_np, axis=1)

        # Generate Kmeans labels
        kmeans_centroids = run_kmeans(k, file_name, max_iter=300, epsilon=1e-4)
        kmeans_labels = np.argmin(
            np.linalg.norm(
                data_points[:, np.newaxis] - np.array(kmeans_centroids), axis=2
            ),
            axis=1,
        )

        # Calculate silhouette scores
        nmf_score = silhouette_score(data_points, nmf_labels)
        kmeans_score = silhouette_score(data_points, kmeans_labels)

        # Print results in the required format
        print(f"nmf: {nmf_score:.4f}")
        print(f"kmeans: {kmeans_score:.4f}")

    except Exception as e:
        print("An Error Has Occurred")
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    main()
