const dashboardData = {
    "cpuCores": 8,
    "metrics": [
        {
            "filter": "Gaussian Blur (Scalar)",
            "threads": 1,
            "serialTime": 0.15,
            "parallelTime": 0.16,
            "speedup": 0.937499,
            "efficiency": 0.937499
        },
        {
            "filter": "Gaussian Blur (Scalar)",
            "threads": 2,
            "serialTime": 0.15,
            "parallelTime": 0.098,
            "speedup": 1.53061,
            "efficiency": 0.765305
        },
        {
            "filter": "Gaussian Blur (Scalar)",
            "threads": 4,
            "serialTime": 0.15,
            "parallelTime": 0.0599999,
            "speedup": 2.5,
            "efficiency": 0.625
        },
        {
            "filter": "Gaussian Blur (Scalar)",
            "threads": 8,
            "serialTime": 0.15,
            "parallelTime": 0.0409999,
            "speedup": 3.65854,
            "efficiency": 0.457318
        },
        {
            "filter": "Gaussian Blur (Scalar)",
            "threads": 16,
            "serialTime": 0.15,
            "parallelTime": 0.0319998,
            "speedup": 4.68752,
            "efficiency": 0.29297
        },
        {
            "filter": "Gaussian Blur (Tiled 64x64)",
            "threads": 1,
            "serialTime": 0.148,
            "parallelTime": 0.146,
            "speedup": 1.0137,
            "efficiency": 1.0137
        },
        {
            "filter": "Gaussian Blur (Tiled 64x64)",
            "threads": 2,
            "serialTime": 0.148,
            "parallelTime": 0.0770001,
            "speedup": 1.92207,
            "efficiency": 0.961037
        },
        {
            "filter": "Gaussian Blur (Tiled 64x64)",
            "threads": 4,
            "serialTime": 0.148,
            "parallelTime": 0.0420001,
            "speedup": 3.5238,
            "efficiency": 0.880951
        },
        {
            "filter": "Gaussian Blur (Tiled 64x64)",
            "threads": 8,
            "serialTime": 0.148,
            "parallelTime": 0.026,
            "speedup": 5.6923,
            "efficiency": 0.711538
        },
        {
            "filter": "Gaussian Blur (Tiled 64x64)",
            "threads": 16,
            "serialTime": 0.148,
            "parallelTime": 0.023,
            "speedup": 6.43478,
            "efficiency": 0.402174
        },
        {
            "filter": "Inversion (Scalar)",
            "threads": 1,
            "serialTime": 0.000999928,
            "parallelTime": 0.00100017,
            "speedup": 0.999762,
            "efficiency": 0.999762
        },
        {
            "filter": "Inversion (Scalar)",
            "threads": 2,
            "serialTime": 0.000999928,
            "parallelTime": 0.000999928,
            "speedup": 1.0,
            "efficiency": 0.5
        },
        {
            "filter": "Inversion (Scalar)",
            "threads": 4,
            "serialTime": 0.000999928,
            "parallelTime": 0.000999928,
            "speedup": 1.0,
            "efficiency": 0.25
        },
        {
            "filter": "Inversion (Scalar)",
            "threads": 8,
            "serialTime": 0.000999928,
            "parallelTime": 0.0,
            "speedup": Infinity,
            "efficiency": Infinity
        },
        {
            "filter": "Inversion (Scalar)",
            "threads": 16,
            "serialTime": 0.000999928,
            "parallelTime": 0.0,
            "speedup": Infinity,
            "efficiency": Infinity
        },
        {
            "filter": "Inversion (AVX2 SIMD)",
            "threads": 1,
            "serialTime": 0.0,
            "parallelTime": 0.0,
            "speedup": NaN,
            "efficiency": NaN
        },
        {
            "filter": "Inversion (AVX2 SIMD)",
            "threads": 2,
            "serialTime": 0.0,
            "parallelTime": 0.0,
            "speedup": NaN,
            "efficiency": NaN
        },
        {
            "filter": "Inversion (AVX2 SIMD)",
            "threads": 4,
            "serialTime": 0.0,
            "parallelTime": 0.0,
            "speedup": NaN,
            "efficiency": NaN
        },
        {
            "filter": "Inversion (AVX2 SIMD)",
            "threads": 8,
            "serialTime": 0.0,
            "parallelTime": 0.0,
            "speedup": NaN,
            "efficiency": NaN
        },
        {
            "filter": "Inversion (AVX2 SIMD)",
            "threads": 16,
            "serialTime": 0.0,
            "parallelTime": 0.0,
            "speedup": NaN,
            "efficiency": NaN
        },
        {
            "filter": "Grayscale",
            "threads": 1,
            "serialTime": 0.00199986,
            "parallelTime": 0.00200009,
            "speedup": 0.999881,
            "efficiency": 0.999881
        },
        {
            "filter": "Grayscale",
            "threads": 2,
            "serialTime": 0.00199986,
            "parallelTime": 0.00100017,
            "speedup": 1.99952,
            "efficiency": 0.999762
        },
        {
            "filter": "Grayscale",
            "threads": 4,
            "serialTime": 0.00199986,
            "parallelTime": 0.000999928,
            "speedup": 2.0,
            "efficiency": 0.5
        },
        {
            "filter": "Grayscale",
            "threads": 8,
            "serialTime": 0.00199986,
            "parallelTime": 0.000999928,
            "speedup": 2.0,
            "efficiency": 0.25
        },
        {
            "filter": "Grayscale",
            "threads": 16,
            "serialTime": 0.00199986,
            "parallelTime": 0.00100017,
            "speedup": 1.99952,
            "efficiency": 0.12497
        },
        {
            "filter": "Sepia tone",
            "threads": 1,
            "serialTime": 0.00399995,
            "parallelTime": 0.00399995,
            "speedup": 1.0,
            "efficiency": 1.0
        },
        {
            "filter": "Sepia tone",
            "threads": 2,
            "serialTime": 0.00399995,
            "parallelTime": 0.00199986,
            "speedup": 2.00012,
            "efficiency": 1.00006
        },
        {
            "filter": "Sepia tone",
            "threads": 4,
            "serialTime": 0.00399995,
            "parallelTime": 0.00100017,
            "speedup": 3.99928,
            "efficiency": 0.999821
        },
        {
            "filter": "Sepia tone",
            "threads": 8,
            "serialTime": 0.00399995,
            "parallelTime": 0.000999928,
            "speedup": 4.00024,
            "efficiency": 0.50003
        },
        {
            "filter": "Sepia tone",
            "threads": 16,
            "serialTime": 0.00399995,
            "parallelTime": 0.000999928,
            "speedup": 4.00024,
            "efficiency": 0.250015
        },
        {
            "filter": "Sharpening",
            "threads": 1,
            "serialTime": 0.0369999,
            "parallelTime": 0.0510001,
            "speedup": 0.725487,
            "efficiency": 0.725487
        },
        {
            "filter": "Sharpening",
            "threads": 2,
            "serialTime": 0.0369999,
            "parallelTime": 0.039,
            "speedup": 0.948716,
            "efficiency": 0.474358
        },
        {
            "filter": "Sharpening",
            "threads": 4,
            "serialTime": 0.0369999,
            "parallelTime": 0.0239999,
            "speedup": 1.54167,
            "efficiency": 0.385417
        },
        {
            "filter": "Sharpening",
            "threads": 8,
            "serialTime": 0.0369999,
            "parallelTime": 0.02,
            "speedup": 1.85,
            "efficiency": 0.23125
        },
        {
            "filter": "Sharpening",
            "threads": 16,
            "serialTime": 0.0369999,
            "parallelTime": 0.0149999,
            "speedup": 2.46668,
            "efficiency": 0.154168
        },
        {
            "filter": "Sobel Edges",
            "threads": 1,
            "serialTime": 0.0780001,
            "parallelTime": 0.113,
            "speedup": 0.690267,
            "efficiency": 0.690267
        },
        {
            "filter": "Sobel Edges",
            "threads": 2,
            "serialTime": 0.0780001,
            "parallelTime": 0.0810001,
            "speedup": 0.962963,
            "efficiency": 0.481481
        },
        {
            "filter": "Sobel Edges",
            "threads": 4,
            "serialTime": 0.0780001,
            "parallelTime": 0.0480001,
            "speedup": 1.625,
            "efficiency": 0.40625
        },
        {
            "filter": "Sobel Edges",
            "threads": 8,
            "serialTime": 0.0780001,
            "parallelTime": 0.04,
            "speedup": 1.95,
            "efficiency": 0.24375
        },
        {
            "filter": "Sobel Edges",
            "threads": 16,
            "serialTime": 0.0780001,
            "parallelTime": 0.0320001,
            "speedup": 2.4375,
            "efficiency": 0.152344
        }
    ]
};
