import pytest

from testsuite.databases import pgsql


# Тест успешной конвертации валют
async def test_currency_conversion(service_client):
    response = await service_client.post(
        '/v1/converter',
        params={
            'from_currency': 'USD',
            'to_currency': 'EUR',
            'amount': '100',
        },
    )
    assert response.status == 200
    assert response.text == 'Converted amount: 85.00 EUR\n'


async def test_missing_parameters(service_client):
    response = await service_client.post('/v1/converter', params={})
    assert response.status == 200
    assert response.text == 'Missing required parameters: from_currency, to_currency, amount\n'


async def test_invalid_amount(service_client):
    response = await service_client.post(
        '/v1/converter',
        params={
            'from_currency': 'USD',
            'to_currency': 'EUR',
            'amount': 'abc',
        },
    )
    assert response.status == 200
    assert response.text == 'Invalid amount parameter\n'


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_conversion_rate_not_found(service_client):
    response = await service_client.post(
        '/v1/converter',
        params={
            'from_currency': 'USD',
            'to_currency': 'INR',
            'amount': '100',
        },
    )
    assert response.status == 200
    assert response.text == 'Conversion rate not found for USD to INR\n'