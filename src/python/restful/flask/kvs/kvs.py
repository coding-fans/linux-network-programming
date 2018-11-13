#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   kvs.py
Author:     Fasion Chan
@contact:   fasionchan@gmail.com
@version:   $Id$

Description:

Changelog:

'''

import json

from collections import (
    OrderedDict,
)
from flask import (
    Flask,
    jsonify,
    request,
)

SIZE_LIMIT = 100
KVS = OrderedDict()

app = Flask(__name__)
app.config['JSON_AS_ASCII'] = False


def get_request_data():
    '''
        获取请求数据

        根据请求数据类型反序列化
    '''

    data = None

    # 类型
    content_type = request.headers['Content-Type']

    try:
        # json
        if content_type == 'application/json':
            data = json.loads(request.data)
    except Exception as exc:
        print(exc)

    return isinstance(data, dict), data


@app.route('/kvs', methods=['POST'])
def create():
    '''
        增
    '''

    # 资源不足，返回错误
    if len(KVS) >= SIZE_LIMIT:
        return jsonify({
            'result': False,
            'message': 'out of resources',
        })

    # 获取请求数据
    result, data = get_request_data()

    # 请求数据错误
    if not result:
        return jsonify({
            'result': False,
            'message': 'bad request data',
        })

    # 取出数据键
    key = data.get('key')
    if not key:
        return jsonify({
            'result': False,
            'message': 'data key missing',
        })

    # 插入资源池并判断状态
    result = KVS.setdefault(key, data) is data

    # 资源已存在
    if not result:
        return jsonify({
            'result': False,
            'message': 'resource exists',
        })

    return jsonify({
        'result': result,
        'data': data,
    })


@app.route('/kvs/<key>', methods=['DELETE'])
def delete(key):
    '''
        删
    '''

    # 删除资源
    data = KVS.pop(key, None)

    # 资源不存在
    if data is None:
        return jsonify({
            'result': False,
            'message': 'resource not exists',
        })

    return jsonify({
        'result': True,
        'data': data,
    })


@app.route('/kvs/<key>', methods=['PUT'])
def update(key):
    '''
        改
    '''

    # 获取请求数据
    result, changes = get_request_data()

    # 请求数据错误
    if not result:
        return jsonify({
            'result': False,
            'message': 'bad request data',
        })

    # 获取资源
    data = KVS.get(key)

    # 资源不存在
    if data is None:
        return jsonify({
            'result': False,
            'message': 'resource not exists',
        })

    # 更新资源
    data.update(changes)

    return jsonify({
        'result': True,
        'data': data,
    })


@app.route('/kvs/<key>')
def retrieve(key):
    '''
        查
    '''

    # 获取资源
    data = KVS.get(key)

    # 资源不存在
    if data is None:
        return jsonify({
            'result': False,
            'message': 'resource not exists',
        })

    return jsonify({
        'result': True,
        'data': data,
    })


@app.route('/kvs')
def search():
    '''
        列
    '''

    skip = int(request.args.get('skip', 0))
    limit = int(request.args.get('limit', 10))
    data = list(KVS.values())[skip:skip+limit]

    return jsonify({
        'result': True,
        'data': data,
        'meta': {
            'total': len(KVS),
            'skip': skip,
            'count': len(data),
        },
    })
