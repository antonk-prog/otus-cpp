#!/usr/bin/env python3
import csv
import io
import statistics
import subprocess
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "bin"
DATASET = ROOT / "input" / "AB_NYC_2019.csv"

HEADER = (
    "id,name,host_id,host_name,neighbourhood_group,neighbourhood,"
    "latitude,longitude,room_type,price,minimum_nights,number_of_reviews,"
    "last_review,reviews_per_month,calculated_host_listings_count,"
    "availability_365\n"
)


def run_mapper(name, text):
    result = subprocess.run(
        [str(BIN / name)],
        input=text,
        text=True,
        capture_output=True,
        check=True,
    )
    return result.stdout


def run_reducer(name, mapper_output):
    lines = sorted(line for line in mapper_output.splitlines() if line)
    result = subprocess.run(
        [str(BIN / name)],
        input="\n".join(lines) + ("\n" if lines else ""),
        text=True,
        capture_output=True,
        check=True,
    )
    return result.stdout


def read_result(output):
    values = {}
    for line in output.splitlines():
        key, value = line.split("\t")
        values[key] = float(value)
    return values


def run_pipeline(mapper, reducer, text):
    return read_result(run_reducer(reducer, run_mapper(mapper, text)))


class MapReduceTest(unittest.TestCase):
    def test_mean_and_population_variance(self):
        text = HEADER
        text += (
            '1,"Home, with comma",10,Host,Brooklyn,Kensington,'
            "40.0,-73.0,Private room,100,1,0,,,1,365\n"
            "2,Second home,11,Host,Manhattan,Midtown,"
            "40.1,-73.1,Entire home/apt,200,1,0,,,1,365\n"
            "3,Third home,12,Host,Queens,Astoria,"
            "40.2,-73.2,Private room,300,1,0,,,1,365\n"
        )

        mean = run_pipeline("mapper_mean", "reducer_mean", text)
        variance = run_pipeline("mapper_variance", "reducer_variance", text)

        self.assertAlmostEqual(mean["mean"], 200.0)
        self.assertAlmostEqual(variance["variance"], 20000.0 / 3.0)

    def test_invalid_rows_are_skipped(self):
        text = HEADER
        text += (
            "1,a,1,h,g,n,0,0,r,,1,0,,,1,1\n"
            "2,a,1,h,g,n,0,0,r,abc,1,0,,,1,1\n"
            "3,a,1,h,g,n,0,0,r,-5,1,0,,,1,1\n"
            "4,a,1,h,g,n,0,0,r\n"
            "5,a,1,h,g,n,0,0,r,10,1,0,,,1,1\n"
            "6,a,1,h,g,n,0,0,r,20,1,0,,,1,1\n"
        )

        mean = run_pipeline("mapper_mean", "reducer_mean", text)
        variance = run_pipeline("mapper_variance", "reducer_variance", text)

        self.assertAlmostEqual(mean["mean"], 15.0)
        self.assertAlmostEqual(variance["variance"], 25.0)

    def test_data_without_header_uses_known_price_column(self):
        text = (
            "1,a,1,h,g,n,0,0,r,40,1,0,,,1,1\n"
            "2,a,1,h,g,n,0,0,r,44,1,0,,,1,1\n"
        )

        mean = run_pipeline("mapper_mean", "reducer_mean", text)
        self.assertAlmostEqual(mean["mean"], 42.0)

    @unittest.skipUnless(DATASET.exists(), "dataset is not available")
    def test_kaggle_dataset(self):
        text = DATASET.read_text()
        prices = []
        for row in csv.DictReader(io.StringIO(text)):
            try:
                prices.append(float(row["price"]))
            except (TypeError, ValueError):
                pass

        expected_mean = statistics.fmean(prices)
        expected_variance = statistics.pvariance(prices)

        mean = run_pipeline("mapper_mean", "reducer_mean", text)
        variance = run_pipeline("mapper_variance", "reducer_variance", text)

        self.assertAlmostEqual(mean["mean"], expected_mean, places=6)
        self.assertAlmostEqual(
            variance["variance"], expected_variance, delta=1e-6
        )


if __name__ == "__main__":
    unittest.main()
