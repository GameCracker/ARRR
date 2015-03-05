import sys
import os
sys.path.append("facerec/py")
from facerec.feature import Fisherfaces
from facerec.distance import EuclideanDistance
from facerec.classifier import NearestNeighbor
from facerec.model import PredictableModel
from facerec.validation import KFoldCrossValidation
from facerec.util import minmax_normalize
from facerec.visual import subplot
from facerec.serialization import save_model, load_model
from facerec.lbp import LPQ, ExtendedLBP
from facedet.detector import CascadedDetector
import numpy as np
import cv2
try:
    from PIL import Image
except ImportError:
    import Image
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import logging
from helper.common import *
from helper.video import *


def read_images(path, sz=None):
    c = 0
    x, y = [], []
    for dirname, dirnames, filenames in os.walk(path):
        print dirname, dirnames, filenames
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                try:
                    im = Image.open(os.path.join(subject_path, filename))
                    im = im.convert("L")
                    if (sz is not None):
                        im = im.resize(sz, Image.ANTIALIAS)
                    x.append(np.asarray(im, dtype=np.uint8))
                    y.append(c)
                except IOError, (errno, strerror):
                    print "I/O error({0}): {1}".format(errno, strerror)
                except:
                    print "Unexpected error:", sys.exc_info()[0]
                    raise
            c = c + 1
    print [x, y]
    return [x, y]

class App(object):
    def __init__(self, model, camera_id, cascade_filename):
        self.model = model
        self.detector = CascadedDetector(cascade_fn=cascade_filename, minNeighbors=5, scaleFactor=1.1)
        self.cam = create_capture(camera_id)

    def run(self):
        while True:
            ret, frame = self.cam.read()
            img = cv2.resize(frame, (frame.shape[1]/2, frame.shape[0]/2), interpolation=cv2.INTER_CUBIC)
            imgout = img.copy()
            for i, r in enumerate(self.detector.detect(img)):
                x0, x1, y0, y1 = r
                face = img[y0:y1, x0:x1]
                face = cv2.cvtColor(face, cv2.COLOR_BGR2GRAY)
                face = cv2.resize(face, self.model.image_size, interpolation=cv2.INTER_CUBIC)
                # prediction = self.model.predic(face)[0]
                cv2.rectangle(imgout, (x0, y0), (x1, y1), (0, 255, 0), 2)
                # draw_str(imgout, (x0-20, y0-20), self.model.subject_names[prediction])
            cv2.imshow('video face recognition', imgout)
            ch = cv2.waitKey(10)
            if ch == 27:
                break

if __name__ == "__main__":
    default_cascade_fn = "/Users/ziqipeng/Dropbox/bci/x/ar/facial_recog/haarcascade_frontalface_alt2.xml"
    App(model=, camera_id=0, cascade_filename=default_cascade_fn).run()
    # out_dir = None
    # if len(sys.argv) < 2:
    #     print "USAGE: classifier.py </path/to/images>"
    #     sys.exit()
    # # Now read in the image data. This must be a valid path!
    # [X,y] = read_images(sys.argv[1])
    # # Then set up a handler for logging:
    # handler = logging.StreamHandler(sys.stdout)
    # formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    # handler.setFormatter(formatter)
    # # Add handler to facerec modules, so we see what's going on inside:
    # logger = logging.getLogger("fisher face recognition")
    # logger.addHandler(handler)
    # logger.setLevel(logging.DEBUG)
    # # Define the Fisherfaces as Feature Extraction method:
    # feature = Fisherfaces()
    # # Define a 1-NN classifier with Euclidean Distance:
    # classifier = NearestNeighbor(dist_metric=EuclideanDistance(), k=1)
    # # Define the model as the combination
    # my_model = PredictableModel(feature=feature, classifier=classifier)
    # # Compute the Fisherfaces on the given data (in X) and labels (in y):
    # my_model.compute(X, y)
    # save_model('model.pkl', my_model)
    # model = load_model('model.pkl')
    # E = []
    # for i in xrange(min(model.feature.eigenvectors.shape[1], 16)):
    #     e = model.feature.eigenvectors[:,i].reshape(X[0].shape)
    #     E.append(minmax_normalize(e,0,255, dtype=np.uint8))
    # # Plot them and store the plot to "python_fisherfaces_fisherfaces.pdf"
    # subplot(title="Fisherfaces", images=E, rows=4, cols=4, sptitle="Fisherface", colormap=cm.jet, filename="fisherfaces.png")
    # # Perform a 10-fold cross validation
    # cv = KFoldCrossValidation(model, k=10)
    # cv.validate(X, y)
    # # And print the result:
    # cv.print_results()

